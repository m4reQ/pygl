# Copyright 2023 Marek 'm4reQ' Mizerski
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the “Software”), to deal
# in the Software without restriction, including without limitation the rights to use,
# copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or
# substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
# FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

import os
import sys
import types
import typing as t
from importlib import abc, machinery


class CombinedExtensionFinder(abc.MetaPathFinder):
    # ignore difference in types as it is inconsistent between `find_spec` and `sys.meta_path` anyway
    def find_spec(fullname: str, # type: ignore
                  path: t.Sequence[str] | None,
                  target: types.ModuleType | None = None) -> machinery.ModuleSpec | None:
        # we dont support subpackages that are deeper than one level from top-level
        # package so throw early if that kind of import is requested.
        # if self.
        if fullname.count('.') > 1:
            return None

        if path is None or len(path) == 0:
            return None

        top_level, target_mod_name = fullname.split('.')

        # find origin filename
        origin: str
        for suf in machinery.EXTENSION_SUFFIXES:
            origin = os.path.join(path[0], f'{top_level}{suf}')
            if os.path.exists(origin):
                break
        else:
            return None

        spec = machinery.ModuleSpec(
            origin=origin,
            name=fullname,
            loader=machinery.ExtensionFileLoader(target_mod_name, path[0]))
        spec.has_location = True

        return spec

sys.meta_path.append(CombinedExtensionFinder) # type: ignore
