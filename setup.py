import os
import shutil

import setuptools as st
from setuptools.command.build_ext import build_ext

CMAKE_BUILD_DIR = 'build/cmake'
CMAKE_CONFIG = 'RelWithDebInfo' if os.path.exists('./.debug') else 'Release'

class pygl_build_ext(build_ext):
    def run(self) -> None:
        cwd = os.getcwd()
        cmake_args = [
            f'-B {CMAKE_BUILD_DIR}',
            f'-S {cwd}',
            f'-DCMAKE_BUILD_TYPE={CMAKE_CONFIG}']

        self.spawn(['cmake'] + cmake_args)
        self.spawn(['cmake', '--build', CMAKE_BUILD_DIR, '--config', CMAKE_CONFIG])

        shutil.copy(
            f'build/cmake/{CMAKE_CONFIG}/pygl.pyd',
            os.path.join(self.build_lib, 'pygl', 'pygl.pyd'))

        if CMAKE_CONFIG == 'RelWithDebInfo':
            shutil.copy(
            f'build/cmake/{CMAKE_CONFIG}/pygl.pdb',
            os.path.join(self.build_lib, 'pygl', 'pygl.pdb'))

        return super().run()

# class pygl_install_data(install_data):
#     def run(self) -> None:
#         install_cmd = self.get_finalized_command('install')
#         self.install_dir = getattr(install_cmd, 'install_lib')

#         return super().run()

st.setup(
    ext_modules=[st.Extension('', [])], # empty extension to force invoke build_ext
    cmdclass={
        'build_ext': pygl_build_ext}) #,
        # 'install_data': pygl_install_data})
