import enum
from collections.abc import Buffer as TSupportsBuffer

class BufferFlags(enum.IntEnum):
    MAP_WRITE_BIT: int
    MAP_READ_BIT: int
    MAP_PERSISTENT_BIT: int
    DYNAMIC_STORAGE_BIT: int

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

class Buffer:
    def __init__(self, size: int, flags: BufferFlags) -> None: ...

    def delete(self) -> None: ...
    def store(self, data: TSupportsBuffer) -> None: ...
    def transfer(self) -> None: ...
    def reset_offset(self) -> None: ...
    def map(self) -> None: ...
    def read(self, out: TSupportsBuffer, size: int, offset: int = 0) -> None: ...
    def bind_base(self, target: BufferBaseTarget, index: int) -> None: ...
    def bind(self, target: BindTarget) -> None: ...

    @property
    def current_offset(self) -> int: ...

    @property
    def id(self) -> int: ...

    @property
    def size(self) -> int: ...
