import enum
import typing as t
from collections.abc import Buffer as TSupportsBuffer

from .math import (Matrix2, Matrix3, Matrix4, Quaternion, Vector2, Vector3,
                   Vector4)

class BufferFlags(enum.IntFlag):
    MAP_WRITE_BIT: int
    MAP_READ_BIT: int
    MAP_PERSISTENT_BIT: int
    MAP_COHERENT_BIT: int
    DYNAMIC_STORAGE_BIT: int
    NONE: int

class BufferBaseTarget(enum.IntEnum):
    ATOMIC_COUNTER_BUFFER: int
    TRANSFORM_FEEDBACK_BUFFER: int
    UNIFORM_BUFFER: int
    SHADER_STORAGE_BUFFER: int

class BindTarget(enum.IntEnum):
    ARRAY_BUFFER: int
    ATOMIC_COUNTER_BUFFER: int
    COPY_READ_BUFFER: int
    COPY_WRITE_BUFFER: int
    DISPATCH_INDIRECT_BUFFER: int
    DRAW_INDIRECT_BUFFER: int
    ELEMENT_ARRAY_BUFFER: int
    PIXEL_PACK_BUFFER: int
    PIXEL_UNPACK_BUFFER: int
    QUERY_BUFFER: int
    SHADER_STORAGE_BUFFER: int
    TEXTURE_BUFFER: int
    TRANSFORM_FEEDBACK_BUFFER: int
    UNIFORM_BUFFER: int

TMatrix = Matrix2 | Matrix3 | Matrix4
TVector = Vector2 | Vector3 | Vector4

class Buffer:
    '''
    A wrapper class for OpenGL buffer. Supports various storage
    types, such as persistent or dynamic mapping.
    '''

    def __init__(self, size: int, flags: BufferFlags, data: TSupportsBuffer | None = None) -> None:
        '''
        Create new buffer `size` bytes big. If the data argument is specified buffer will be populated with
        the provided data. If the data size is bigger than the `size` argument specified the exception
        will be thrown.

        Buffer constructor checks for rough validity of the `flags` argument to ensure that later oparations will
        not fail. It doesn't mean however that it checks for all incorrect usage scenarios, only those that would
        potentially result in a crash.
        '''

    @t.overload
    def store(self, data: TSupportsBuffer, offset: int | None = None) -> None: ...

    @t.overload
    def store(self, data: TMatrix, offset: int | None = None) -> None: ...

    @t.overload
    def store(self, data: TVector, offset: int | None = None) -> None: ...

    @t.overload
    def store(self, data: Quaternion, offset: int | None = None) -> None: ...

    @t.overload
    def store(self, data: int, offset: int | None = None) -> None: ...

    @t.overload
    def store(self, data: float, offset: int | None = None) -> None: ...

    def store_address(self, address: int, size: int) -> None: ...

    def delete(self) -> None: ...
    def transfer(self) -> None: ...
    def reset_offset(self) -> None: ...
    def map(self) -> None: ...
    def read(self, out: TSupportsBuffer, size: int, offset: int = 0) -> None: ...
    def bind_base(self, target: BufferBaseTarget, index: int) -> None: ...
    def bind(self, target: BindTarget) -> None: ...

    @property
    def current_offset(self) -> int: ...

    @current_offset.setter
    def current_offset(self, value: int) -> None: ...

    @property
    def id(self) -> int: ...

    @property
    def size(self) -> int: ...
