import enum
import typing as t
from collections.abc import Buffer as TSupportsBuffer

from .commands import GLType
from .textures import MagFilter, MinFilter

class AttachmentFormat(enum.IntEnum):
    RGBA8 = t.cast(int, ...)
    RGB8 = t.cast(int, ...)
    RG8 = t.cast(int, ...)
    R8 = t.cast(int, ...)
    RGBA16 = t.cast(int, ...)
    RGB16 = t.cast(int, ...)
    RG16 = t.cast(int, ...)
    R16 = t.cast(int, ...)
    RGBA16F = t.cast(int, ...)
    RGB16F = t.cast(int, ...)
    RG16F = t.cast(int, ...)
    R16F = t.cast(int, ...)
    RGBA32F = t.cast(int, ...)
    RGB32F = t.cast(int, ...)
    RG32F = t.cast(int, ...)
    R32F = t.cast(int, ...)
    RGBA8I = t.cast(int, ...)
    RGB8I = t.cast(int, ...)
    RG8I = t.cast(int, ...)
    R8I = t.cast(int, ...)
    RGBA16I = t.cast(int, ...)
    RGB16I = t.cast(int, ...)
    RG16I = t.cast(int, ...)
    R16I = t.cast(int, ...)
    RGBA32I = t.cast(int, ...)
    RGB32I = t.cast(int, ...)
    RG32I = t.cast(int, ...)
    R32I = t.cast(int, ...)
    RGBA8UI = t.cast(int, ...)
    RGB8UI = t.cast(int, ...)
    RG8UI = t.cast(int, ...)
    R8UI = t.cast(int, ...)
    RGBA16UI = t.cast(int, ...)
    RGB16UI = t.cast(int, ...)
    RG16UI = t.cast(int, ...)
    R16UI = t.cast(int, ...)
    RGBA32UI = t.cast(int, ...)
    RGB32UI = t.cast(int, ...)
    RG32UI = t.cast(int, ...)
    R32UI = t.cast(int, ...)
    RGBA4 = t.cast(int, ...)
    RGB5_A1 = t.cast(int, ...)
    RGB565 = t.cast(int, ...)
    RGB10_A2 = t.cast(int, ...)
    RGB10_A2UI = t.cast(int, ...)
    R11F_G11F_B10F = t.cast(int, ...)
    SRGB8_ALPHA8 = t.cast(int, ...)
    DEPTH_COMPONENT16 = t.cast(int, ...)
    DEPTH_COMPONENT24 = t.cast(int, ...)
    DEPTH_COMPONENT32F = t.cast(int, ...)
    DEPTH24_STENCIL8 = t.cast(int, ...)
    DEPTH32F_STENCIL8 = t.cast(int, ...)
    STENCIL_INDEX8 = t.cast(int, ...)

class AttachmentPoint(enum.IntEnum):
    DEPTH_ATTACHMENT = t.cast(int, ...)
    STENCIL_ATTACHMENT = t.cast(int, ...)
    DEPTH_STENCIL_ATTACHMENT = t.cast(int, ...)

class FramebufferAttachment:
    width: int
    height: int
    format: int
    attachment: int
    samples: int
    min_filter: int
    mag_filter: int
    use_renderbuffer: bool
    is_writable: bool

    def __init__(self,
                 width: int,
                 height: int,
                 format: AttachmentFormat,
                 attachment: AttachmentPoint | int,
                 samples: int = 1,
                 min_filter: MinFilter = MinFilter.NEAREST,
                 mag_filter: MagFilter = MagFilter.NEAREST,
                 use_renderbuffer: bool = False,
                 is_writable: bool = True) -> None: ...

    @property
    def is_resizable(self) -> bool: ...

    @property
    def is_depth_attachment(self) -> bool: ...

    @property
    def is_color_attachment(self) -> bool: ...

    @property
    def is_multisampled(self) -> bool: ...

    @property
    def size(self) -> tuple[int, int]: ...

    @size.setter
    def size(self, value: tuple[int, int]) -> None: ...

class Framebuffer:
    def __init__(self, attachments: list[FramebufferAttachment], width: int, height: int) -> None: ...
    def delete(self) -> None: ...
    def bind(self) -> None: ...
    def unbind(self) -> None: ...
    def resize(self, width: int, height: int, /) -> None: ...
    def get_attachment(self, attachment_point: AttachmentPoint | int, /) -> FramebufferAttachment: ...
    def get_attachment_id(self, attachment_point: AttachmentPoint | int, /) -> int: ...
    def clear_color_attachment(self, clear_value: TSupportsBuffer, draw_buffer: int, value_type: GLType, /) -> None: ...
    def clear_depth_attachment(self, depth_value: float, /) -> None: ...
    def clear_stencil_attachment(self, stencil_value: int, /) -> None: ...
    def clear_depth_stencil_attachment(self, depth_value: float, stencil_value: int, /) -> None: ...

    @property
    def id(self) -> int: ...

    @property
    def width(self) -> int: ...

    @property
    def height(self) -> int: ...

    @property
    def size(self) -> tuple[int, int]: ...

    @property
    def attachments_count(self) -> int: ...
