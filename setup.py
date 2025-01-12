import os
import shutil
import subprocess

import setuptools

IS_DEBUG = False
CMAKE_BUILD_TYPE = 'RelWithDebInfo' if IS_DEBUG else 'Release'
CMAKE_DIR = './build/cmake'
CMAKE_BUILD_DIR = f'{CMAKE_DIR}/{CMAKE_BUILD_TYPE}'

class PyglDist(setuptools.Distribution):
    def has_ext_modules(self):
        return True

subprocess.call(f'cmake -S . -B {CMAKE_DIR} -DCMAKE_BUILD_TYPE={CMAKE_BUILD_TYPE} -DCMAKE_VERBOSE_MAKEFILE=ON')
subprocess.call(f'cmake --build {CMAKE_DIR} --config {CMAKE_BUILD_TYPE}')

# temporarily copy resulting dll into package directory bcs setuptools suck
shutil.copy(f'{CMAKE_BUILD_DIR}/pygl.pyd', 'pygl/pygl.pyd')

setuptools.setup(
    name='pygl',
    version='0.0.1',
    packages=['pygl'],
    classifiers=[
        'Topic :: Multimedia :: Graphics',
        'Topic :: Multimedia :: Graphics :: 3D Rendering',
        'Topic :: Software Development :: Libraries',
        'Intended Audience :: Developers',
        'Programming Language :: Python :: 3 :: Only',
        'Programming Language :: Python :: 3.12',
        'Programming Language :: Python :: 3.13',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: POSIX :: Linux',
        'Operating System :: MacOS'],
    description='A simple OpenGL utility library to speed up development.',
    author='Marek `m4reQ` Mizerski',
    url='https://github.com/m4reQ/pygl',
    package_data={'pygl': ['pygl.pyd']},
    distclass=PyglDist)

# remove temporary copy of dll
os.remove('pygl/pygl.pyd')
