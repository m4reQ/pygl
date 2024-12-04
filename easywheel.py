import argparse
import dataclasses
import glob
import hashlib
import json
import os
import re
import shutil
import tempfile
import typing as t
import zipfile

import yaml
from packaging import tags

METADATA_VERSION = '2.1'
WHL_VERSION = '1.0'

@dataclasses.dataclass
class _FileTag:
    filepath: str
    sha256_hash: str
    size: int

@dataclasses.dataclass
class _PackagedFile:
    origin: str
    destination: str

    def __init__(self, origin: str, destination: str | None = None) -> None:
        self.origin = _normalize_path(origin)
        self.destination = _normalize_path(destination) if destination is not None else self.origin

@dataclasses.dataclass
class Package:
    name: str
    version: str
    python_required: str
    files: list[_PackagedFile]
    projects: list[str]
    authors: list[str] = dataclasses.field(default_factory=list)
    license: str = ''
    summary: str = ''
    urls: dict[str, str] = dataclasses.field(default_factory=dict)
    classifiers: list[str] = dataclasses.field(default_factory=list)
    keywords: list[str] = dataclasses.field(default_factory=list)
    platforms: list[str] = dataclasses.field(default_factory=list)
    is_pure: bool = True

    @classmethod
    def from_file(cls, filepath: str) -> t.Self:
        with open(filepath, 'r') as f:
            data: dict[str, t.Any] = yaml.safe_load(f)

        return cls(
            data['name'],
            data['version'],
            data['python'],
            _gather_project_files(data['files']),
            data['projects'],
            data.get('authors', []),
            _process_item(data.get('license', None)),
            _process_item(data.get('summary', None)),
            data.get('urls', []),
            data.get('classifiers', []),
            data.get('keywords', []),
            data.get('platforms', []),
            data.get('is_pure', True))

    def build(self, build_dir: str | None = None) -> str:
        compat_tag = _create_compat_tag(self.python_required, self.is_pure)
        whl_name = f'{self.name}-{self.version}-{compat_tag}'

        if build_dir is None:
            build_dir = tempfile.gettempdir()

        pkg_dir = os.path.join(build_dir, whl_name)
        dist_info_dir = os.path.join(pkg_dir, f'{self.name}-{self.version}.dist-info')

        metadata_filepath = os.path.join(dist_info_dir, 'METADATA')
        os.makedirs(os.path.dirname(metadata_filepath), exist_ok=True)

        with open(metadata_filepath, 'w+') as f:
            f.write(f'Metadata-Version: {METADATA_VERSION}\n')
            f.write(f'Name: {self.name}\n')
            f.write(f'Version: {self.version}\n')
            f.write(f'Summary: {self.summary}\n')

            if (home_url := self.urls.get('home', None)) is not None:
                f.write(f'Home-page: {home_url}\n')

            for author in self.authors:
                f.write(f'Author: {author}\n')

            f.write(f'Keywords: {' '.join(self.keywords)}\n')

            for classifier in self.classifiers:
                f.write(f'Classifier: {classifier}\n')

        top_level_filepath = os.path.join(dist_info_dir, 'top_level.txt')
        os.makedirs(os.path.dirname(top_level_filepath), exist_ok=True)

        with open(top_level_filepath, 'w+') as f:
            for project in self.projects:
                f.write(f'{project}\n')

        wheel_filepath = os.path.join(dist_info_dir, 'WHEEL')
        os.makedirs(os.path.dirname(wheel_filepath), exist_ok=True)

        with open(wheel_filepath, 'w+') as f:
            f.write(f'Wheel-Version: {WHL_VERSION}\n')
            f.write('Generator: easywheel\n')
            f.write(f'Root-Is-Purelib: {self.is_pure}\n') # TODO detect if only python files are present
            f.write(f'Tag: {compat_tag}\n')

        file_tags: list[_FileTag] = [
            _create_file_tag(_PackagedFile(metadata_filepath)),
            _create_file_tag(_PackagedFile(top_level_filepath)),
            _create_file_tag(_PackagedFile(wheel_filepath))]
        for file in self.files:
            file_tags.append(_create_file_tag(file))

        record_filepath = os.path.join(dist_info_dir, 'RECORD')
        os.makedirs(os.path.dirname(record_filepath), exist_ok=True)

        with open(record_filepath, 'w+') as f:
            for tag in file_tags:
                f.write(f'{tag.filepath},sha256={tag.sha256_hash},{tag.size}\n')

            f.write(f'{_normalize_path(record_filepath)},,')

        copied_files = list[str]()
        for file in self.files:
            dirname = os.path.dirname(file.destination)
            if dirname != '':
                os.makedirs(os.path.join(pkg_dir, dirname), exist_ok=True)

            dst_filepath = os.path.join(pkg_dir, file.destination)
            shutil.copyfile(file.origin, dst_filepath)

            copied_files.append(dst_filepath)

        whl_filepath = os.path.join(build_dir, f'{whl_name}.whl')
        os.makedirs(os.path.dirname(whl_filepath), exist_ok=True)

        with zipfile.ZipFile(whl_filepath, 'x') as whl_file:
            whl_file.write(metadata_filepath, arcname=os.path.relpath(metadata_filepath, pkg_dir))
            whl_file.write(top_level_filepath, arcname=os.path.relpath(top_level_filepath, pkg_dir))
            whl_file.write(record_filepath, arcname=os.path.relpath(record_filepath, pkg_dir))
            whl_file.write(wheel_filepath, arcname=os.path.relpath(wheel_filepath, pkg_dir))

            for copied_file in copied_files:
                whl_file.write(copied_file, arcname=os.path.relpath(copied_file, pkg_dir))

        data = {
            'whl_filepath': _normalize_path(whl_filepath),
            'whl_filename': os.path.basename(whl_filepath)
        }
        return json.dumps(data)

def _normalize_path(filepath: str) -> str:
    return filepath.lstrip('./').replace('\\', '/')

def _create_file_tag(file: _PackagedFile) -> _FileTag:
    with open(file.origin, 'rb') as f:
        content = f.read()

    return _FileTag(
        file.destination,
        hashlib.sha256(content).hexdigest(),
        len(content))

def _process_item(filepath_or_str: str | None) -> str:
    if filepath_or_str is None:
        return ''

    if filepath_or_str.startswith('!f'):
        filepath_or_str = filepath_or_str.lstrip('!f').strip()
        with open(filepath_or_str, 'r') as f:
            return f.read()

    return filepath_or_str

def _gather_project_files(candidates: list[str]) -> list[_PackagedFile]:
    files = list[_PackagedFile]()

    for candidate in candidates:
        env_replaces = re.findall(r'\$(.+?)\$', candidate)
        if len(env_replaces) != 0:
            for env_replace in env_replaces:
                try:
                    replacement = os.environ[env_replace].strip()
                except KeyError:
                    raise RuntimeError(f'Failed to find replacement environment variable "{env_replace}"')

                candidate = candidate.replace(f'${env_replace}$', replacement)

        if candidate.startswith('!g'):
            if ':' in candidate:
                raise RuntimeError('Cannot use destination filepath replacement with glob expressions.')

            candidate = candidate.lstrip('!g').strip()

            files.extend(_PackagedFile(x) for x in glob.glob(candidate, recursive=True))
        else:
            filepaths = candidate.split(':')
            if len(filepaths) == 1:
                files.append(_PackagedFile(filepaths[0]))
            else:
                if len(filepaths) != 2:
                    raise RuntimeError('Invalid destination filepath replacement, must be in form of <filepath>:<destination_filepath>')

                files.append(
                    _PackagedFile(
                        filepaths[0].strip(),
                        filepaths[1].strip()))

    return files

def _create_compat_tag(python_version: str, is_pure: bool) -> str:
    require_exact_version = False
    if python_version.startswith('=='):
        require_exact_version = True
        python_version = python_version.lstrip('==')
    elif python_version.startswith('>='):
        python_version = python_version.lstrip('>=')

    version = [int(x) for x in python_version.split('.')]
    if len(version) > 3 or len(version) == 0:
        raise RuntimeError(f'Invalid python version: {version}')

    if is_pure:
        supported = tags.compatible_tags(version)
    else:
        supported = tags.cpython_tags(version)

    for tag in supported:
        if not is_pure and tag.abi == 'none' or tag.interpreter == f'cp{version[0]}{version[1]}':
            continue

        return str(tag)

    raise RuntimeError('Couldn\'t find suitable compatibility tag. This should not happen and is most likely a bug.')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='A simple Python wheel generator')
    parser.add_argument(
        '-S',
        default='.',
        required=False,
        dest='source',
        help='A source directory to be packaged. Must contain package.yml file.')
    parser.add_argument(
        '-B',
        default='./build',
        required=False,
        dest='build',
        help='Build directory to which all intermediate files will be generated.')

    args = parser.parse_args()

    old_working_dir = os.getcwd()
    os.chdir(args.source)

    pkg = Package.from_file('package.yml')

    os.chdir(old_working_dir)
    whl_info = pkg.build(args.build)

    print(whl_filepath)
