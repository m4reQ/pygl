import enum

from .buffers import Buffer, MappedBuffer

class AttribType(enum.IntEnum):
    GL_FLOAT: int
    GL_HALF_FLOAT: int
    GL_DOUBLE: int
    GL_BYTE: int
    GL_UNSIGNED_BYTE: int
    GL_SHORT: int
    GL_UNSIGNED_SHORT: int
    GL_INT: int
    GL_UNSIGNED_INT: int

class VertexDescriptor:
    attrib_index: int
    type: int
    count: int
    rows: int = 0
    is_normalized: bool = False

    def __init__(self,
                 attrib_index: int,
                 type: int,
                 count: int,
                 rows: int = 0,
                 is_normalized: bool = False) -> None: ...

class VertexInput:
    buffer_id: int
    stride: int
    descriptors: list[VertexDescriptor]
    offset: int = 0
    divisor: int = 0

    def __init__(self,
                 buffer: Buffer | MappedBuffer,
                 stride: int,
                 descriptors: list[VertexDescriptor],
                 offset: int = 0,
                 divisor: int = 0) -> None: ...

class VertexArray:
    id: int

    def __init__(self, layout: list[VertexInput], element_buffer: Buffer | MappedBuffer | None = None) -> None: ...

    def delete(self) -> None: ...
    def bind(self) -> None: ...
