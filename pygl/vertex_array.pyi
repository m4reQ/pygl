import enum

from .buffers import Buffer

class AttribType(enum.IntEnum):
    FLOAT: int
    HALF_FLOAT: int
    DOUBLE: int
    BYTE: int
    UNSIGNED_BYTE: int
    SHORT: int
    UNSIGNED_SHORT: int
    INT: int
    UNSIGNED_INT: int

class VertexDescriptor:
    attrib_index: int
    type: int
    count: int
    rows: int = 1
    is_normalized: bool = False

    def __init__(self,
                 attrib_index: int,
                 type: AttribType,
                 count: int,
                 rows: int = 1,
                 is_normalized: bool = False) -> None: ...

class VertexInput:
    buffer_id: int
    stride: int
    descriptors: list[VertexDescriptor]
    offset: int = 0
    divisor: int = 0

    def __init__(self,
                 buffer: Buffer,
                 stride: int,
                 descriptors: list[VertexDescriptor],
                 offset: int = 0,
                 divisor: int = 0) -> None: ...

class VertexArray:
    id: int

    def __init__(self, layout: list[VertexInput], element_buffer: Buffer | None = None) -> None: ...

    def delete(self) -> None: ...
    def bind(self) -> None: ...
    def bind_vertex_buffer(self, buffer: Buffer, index: int, stride: int, offset: int = 0, divisor: int = 0) -> None: ...
    def bind_index_buffer(self, buffer: Buffer) -> None: ...
