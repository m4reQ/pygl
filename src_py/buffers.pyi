import enum
from collections.abc import Buffer as TSupportsBuffer

class BufferFlags(enum.IntEnum):
    MAP_WRITE_BIT: int
    MAP_READ_BIT: int
    MAP_PERSISTENT_BIT: int
    DYNAMIC_STORAGE_BIT: int

class Buffer:
    def __init__(self, size: int, flags: BufferFlags) -> None: ...

    def delete(self) -> None: ...
    def store(self, data: TSupportsBuffer) -> None: ...
    def transfer(self) -> None: ...
    def reset_offset(self) -> None: ...
    def map(self) -> None: ...
    def read(self, out: TSupportsBuffer, size: int, offset: int = 0) -> None: ...

    @property
    def current_offset(self) -> int: ...

    @property
    def id(self) -> int: ...

    @property
    def size(self) -> int: ...
