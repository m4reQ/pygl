import argparse
import dataclasses
import glob
import hashlib
import itertools
import json
import os
import re
import sys
import tempfile
import typing as t
import zipfile

from packaging import tags

METADATA_VERSION = '2.1'
WHL_VERSION = '1.0'

@dataclasses.dataclass
class _FileRecord:
    filepath: str
    sha256_hash: str
    size: int

    @classmethod
    def from_filepath(cls, filepath: str, parent_relative: bool = False) -> t.Self:
        with open(filepath, 'rb') as f:
            data = f.read()

        if parent_relative:
            filepath = _get_filepath_parent_relative(filepath)

        return cls(
            filepath=_normalize_path(filepath),
            sha256_hash=hashlib.sha256(data).hexdigest(),
            size=len(data))

@dataclasses.dataclass
class _FileDef:
    origin: str
    destination: str | None = None

@dataclasses.dataclass
class Metadata:
    summary: str = ''
    license: str = ''
    author: str = ''
    urls: dict[str, str] = dataclasses.field(default_factory=dict)
    platforms: list[str] = dataclasses.field(default_factory=list)
    classifiers: list[str] = dataclasses.field(default_factory=list)
    keywords: list[str] = dataclasses.field(default_factory=list)

    @classmethod
    def from_dict(cls, data: t.Mapping[str, t.Any]) -> t.Self:
        return cls(
            summary=data.get('summary', ''),
            license=data.get('license', ''),
            author=data.get('author', ''),
            urls=data.get('urls', {}),
            platforms=data.get('platforms', []),
            classifiers=data.get('classifiers', []),
            keywords=data.get('keywords', []))

@dataclasses.dataclass
class Package:
    name: str
    files: list[_FileDef]

    @classmethod
    def from_dict(cls, data: t.Mapping[str, t.Any]) -> t.Self:
        return cls(
            data['name'],
            _process_file_defs(data['files']))

@dataclasses.dataclass
class Project:
    name: str
    version: str
    python_version: str
    packages: list[Package]
    metadata: Metadata | None = None
    is_pure: bool = True
    use_stable_abi: bool = False
    whl_name_override: str | None = None

    @classmethod
    def from_file(cls, filepath: str) -> t.Self:
        with open(filepath, 'r') as f:
            data: dict[str, t.Any] = json.load(f)

        metadata: Metadata | None = None
        if (metadata_data := data.get('metadata', None)) is not None:
            metadata = Metadata.from_dict(metadata_data)

        packages = [Package.from_dict(x) for x in data['packages']]

        is_pure = True
        use_stable_abi = False
        whl_name_override: str | None = None
        if (build_data := data.get('build', None)) is not None:
            is_pure = build_data.get('is_pure', True)
            use_stable_abi = build_data.get('use_stable_api', False)
            whl_name_override = build_data.get('whl_name_override', None)
        else:
            is_pure = not any(
                os.path.splitext(file.origin)[1] in ('.pyd', '.dll', '.so')
                for file in itertools.chain(*(x.files for x in packages)))

        return cls(
            name=data['name'],
            version=data['version'],
            python_version=data['python'],
            packages=packages,
            metadata=metadata,
            is_pure=is_pure,
            use_stable_abi=use_stable_abi,
            whl_name_override=whl_name_override)

    def get_compat_tag(self) -> str:
        if self.is_pure:
            version_str = f'py{sys.version_info.major}{sys.version_info.minor}'
            tag = next(
                x
                for x in tags.sys_tags()
                if x.abi == 'none' and x.platform == 'any' and x.interpreter == version_str)
            return str(tag)

        version_str = f'cp{sys.version_info.major}{sys.version_info.minor}'
        required_abi = f'abi{sys.version_info.major}' if self.use_stable_abi else version_str
        tag = next(
            x
            for x in tags.sys_tags()
            if x.abi == required_abi and x.platform != 'any' and x.interpreter == version_str)

        return str(tag)

    def get_whl_name(self, compat_tag: str | None = None) -> str:
        if self.whl_name_override is not None:
            return self.whl_name_override

        compat_tag = compat_tag or self.get_compat_tag()
        return f'{self.name}-{self.version}-{compat_tag}'

    def write_metadata(self, metadata_filepath: str) -> None:
        assert self.metadata is not None

        os.makedirs(os.path.dirname(metadata_filepath), exist_ok=True)

        metadata = self.metadata

        with open(metadata_filepath, 'w+') as f:
            f.write(f'Metadata-Version: {METADATA_VERSION}\n')
            f.write(f'Name: {self.name}\n')
            f.write(f'Version: {self.version}\n')
            f.write(f'Summary: {metadata.summary}\n')
            f.write(f'Author: {metadata.author}\n')

            if len(metadata.keywords) != 0:
                f.write(f'Keywords: {','.join(metadata.keywords)}\n')

            for classifier in metadata.classifiers:
                f.write(f'Classifier: {classifier}\n')

            for name, url in metadata.urls.items():
                f.write(f'Project-URL: {name}, {url}\n')

    def write_top_level(self, top_level_filepath: str) -> None:
        os.makedirs(os.path.dirname(top_level_filepath), exist_ok=True)

        with open(top_level_filepath, 'w+') as f:
            for package in self.packages:
                f.write(f'{package.name}\n')

    def write_wheel(self, wheel_filepath: str, compat_tag: str | None = None) -> None:
        compat_tag = compat_tag or self.get_compat_tag()

        os.makedirs(os.path.dirname(wheel_filepath), exist_ok=True)

        with open(wheel_filepath, 'w+') as f:
            f.write(f'Wheel-Version: {WHL_VERSION}\n')
            f.write('Generator: easywheel\n')
            f.write(f'Root-Is-Purelib: {self.is_pure}\n')
            f.write(f'Tag: {compat_tag}\n')

    def write_record(self,
                     metadata_filepath: str,
                     top_level_filepath: str,
                     wheel_filepath: str,
                     record_filepath: str) -> None:
        os.makedirs(os.path.dirname(record_filepath), exist_ok=True)

        records = [
            _FileRecord.from_filepath(x.origin)
            for x in itertools.chain(*(x.files for x in self.packages))]

        records.append(_FileRecord.from_filepath(metadata_filepath))
        records.append(_FileRecord.from_filepath(top_level_filepath))
        records.append(_FileRecord.from_filepath(wheel_filepath))

        with open(record_filepath, 'w+') as f:
            for record in records:
                f.write(f'{record.filepath},sha256={record.sha256_hash},{record.size}\n')

            f.write(f'{_normalize_path(record_filepath)},,')

    def write_archive(self,
                      metadata_filepath: str,
                      top_level_filepath: str,
                      wheel_filepath: str,
                      record_filepath: str,
                      whl_filepath: str,
                      pkg_dir: str) -> None:
        with zipfile.ZipFile(whl_filepath, 'x') as archive:
            archive.write(metadata_filepath, arcname=os.path.relpath(metadata_filepath, pkg_dir))
            archive.write(top_level_filepath, arcname=os.path.relpath(top_level_filepath, pkg_dir))
            archive.write(record_filepath, arcname=os.path.relpath(record_filepath, pkg_dir))
            archive.write(wheel_filepath, arcname=os.path.relpath(wheel_filepath, pkg_dir))

            for package in self.packages:
                for file in package.files:
                    archive.write(file.origin, file.destination)

    def build(self, build_dir: str | None = None) -> str:
        if build_dir is None:
            build_dir = tempfile.gettempdir()

        compat_tag = self.get_compat_tag()
        whl_name = self.get_whl_name(compat_tag)

        pkg_dir = os.path.join(build_dir, whl_name)
        dist_info_dir = os.path.join(pkg_dir, f'{self.name}-{self.version}.dist-info')

        if self.metadata is not None:
            metadata_filepath = os.path.join(dist_info_dir, 'METADATA')
            self.write_metadata(metadata_filepath)

        top_level_filepath = os.path.join(dist_info_dir, 'top_level.txt')
        self.write_top_level(top_level_filepath)

        wheel_filepath = os.path.join(dist_info_dir, 'WHEEL')
        self.write_wheel(wheel_filepath)

        record_filepath = os.path.join(dist_info_dir, 'RECORD')
        self.write_record(
            metadata_filepath,
            top_level_filepath,
            wheel_filepath,
            record_filepath)

        whl_filepath = os.path.join(build_dir, f'{whl_name}.whl')
        os.makedirs(os.path.dirname(whl_filepath), exist_ok=True)

        self.write_archive(
            metadata_filepath,
            top_level_filepath,
            wheel_filepath,
            record_filepath,
            whl_filepath,
            pkg_dir)

        return whl_filepath

def _normalize_path(filepath: str) -> str:
    return filepath.lstrip('./').replace('\\', '/')

def _get_filepath_parent_relative(filepath: str) -> str:
    filename = os.path.basename(filepath)
    parent = os.path.basename(os.path.dirname(filepath))

    return os.path.join(parent, filename)

def _process_file_defs(file_defs: list[str]) -> list[_FileDef]:
    result = list[_FileDef]()

    for _def in file_defs:
        env_replace_candidates = re.findall(r'\$(.+?)\$', _def)
        for candidate in env_replace_candidates:
            try:
                replacement = os.environ[candidate]
            except KeyError:
                raise RuntimeError(f'Could not find environment variable {candidate} for use as replacement in "{_def}"')

            _def = _def.replace(f'${candidate}$', replacement)

        if _def.startswith('!g'):
            if ':' in _def:
                raise RuntimeError('Cannot use destination filepath replacement with glob expressions.')

            _def = _def[2:].strip()
            result.extend(_FileDef(x) for x in glob.glob(_def))

            continue

        if ':' in _def:
            # TODO Catch multiple : provided
            origin, dest = _def.split(':')
            origin = origin.strip()
            dest = dest.strip()

            result.append(_FileDef(origin, dest))

    return result

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
    parser.add_argument(
        '-I',
        default=False,
        dest='output_info',
        help='If used, the command will output more detailed informations about the build process in the JSON format. Otherwise command will only output created file name.')

    args = parser.parse_args()

    old_working_dir = os.getcwd()
    os.chdir(args.source)

    project = Project.from_file('package.json')

    os.chdir(old_working_dir)
    whl_filepath = project.build(args.build)

    print(whl_filepath)
