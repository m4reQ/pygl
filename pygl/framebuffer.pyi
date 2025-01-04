import enum
import typing as t
from collections.abc import Buffer as TSupportsBuffer

from .textures import MagFilter, MinFilter

class AttachmentFormat(enum.IntEnum):
    RGBA8: int
    RGB8: int
    RG8: int
    R8: int
    RGBA16: int
    RGB16: int
    RG16: int
    R16: int
    RGBA16F: int
    RGB16F: int
    RG16F: int
    R16F: int
    RGBA32F: int
    RGB32F: int
    RG32F: int
    R32F: int
    RGBA8I: int
    RGB8I: int
    RG8I: int
    R8I: int
    RGBA16I: int
    RGB16I: int
    RG16I: int
    R16I: int
    RGBA32I: int
    RGB32I: int
    RG32I: int
    R32I: int
    RGBA8UI: int
    RGB8UI: int
    RG8UI: int
    R8UI: int
    RGBA16UI: int
    RGB16UI: int
    RG16UI: int
    R16UI: int
    RGBA32UI: int
    RGB32UI: int
    RG32UI: int
    R32UI: int
    RGBA4: int
    RGB5_A1: int
    RGB565: int
    RGB10_A2: int
    RGB10_A2UI: int
    R11F_G11F_B10F: int
    SRGB8_ALPHA8: int
    DEPTH_COMPONENT16: int
    DEPTH_COMPONENT24: int
    DEPTH_COMPONENT32F: int
    DEPTH24_STENCIL8: int
    DEPTH32F_STENCIL8: int
    STENCIL_INDEX8: int

class Attachment(enum.IntEnum):
    DEPTH_ATTACHMENT: int
    STENCIL_ATTACHMENT: int
    DEPTH_STENCIL_ATTACHMENT: int

class RenderbufferAttachment:
    width: int
    height: int
    format: int
    attachment: int
    samples: int
    is_resizable: bool

    def __init__(self,
                 width: int,
                 height: int,
                 format: AttachmentFormat,
                 attachment: Attachment | int,
                 samples: int = 1,
                 is_resizable: bool = True) -> None: ...

    @property
    def is_multisampled(self) -> bool: ...

    @property
    def is_depth_attachment(self) -> bool: ...

    @property
    def size(self) -> tuple[int, int]: ...

class TextureAttachment:
    width: int
    height: int
    format: int
    attachment: int
    samples: int
    min_filter: int
    mag_filter: int
    is_resizable: bool

    def __init__(self,
                 width: int,
                 height: int,
                 format: AttachmentFormat,
                 attachment: Attachment | int,
                 samples: int = 1,
                 min_filter: MinFilter = MinFilter.LINEAR,
                 mag_filter: MagFilter = MagFilter.LINEAR,
                 is_resizable: bool = True) -> None: ...

    @property
    def is_multisampled(self) -> bool: ...

    @property
    def is_depth_attachment(self) -> bool: ...

    @property
    def size(self) -> tuple[int, int]: ...

TAttachment = TextureAttachment | RenderbufferAttachment

class Framebuffer:
    def __init__(self, attachments: t.Sequence[TAttachment], width: int, height: int) -> None: ...

    def bind(self) -> None: ...
    def unbind(self) -> None: ...
    def delete(self) -> None: ...
    def resize(self, width: int, height: int) -> None: ...
    def get_attachment_id(self, attachment: Attachment | int) -> int: ...
    # TODO Add missing framebuffer functions
    # def clear(self, r: float, g: float, b: float, a: float) -> None: ...
    # def read_attachment(self, attachment: Attachment | int, buffer: TSupportsBuffer) -> None: ...
    # def read_pixel(self, attachment: Attachment | int, x: int, y: int) -> tuple[int, ...]: ...

    @property
    def size(self) -> tuple[int, int]: ...

    @property
    def id(self) -> int: ...

    @property
    def width(self) -> int: ...

    @property
    def height(self) -> int: ...

    @property
    def specs(self) -> list[TAttachment]: ...
