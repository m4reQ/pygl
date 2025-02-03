import enum
import typing as t
from collections.abc import Buffer as TSupportsBuffer

from .buffers import Buffer

class TextureTarget(enum.IntEnum):
    TEXTURE_1D = t.cast(int, ...)
    TEXTURE_2D = t.cast(int, ...)
    TEXTURE_3D = t.cast(int, ...)
    TEXTURE_1D_ARRAY = t.cast(int, ...)
    TEXTURE_2D_ARRAY = t.cast(int, ...)
    TEXTURE_RECTANGLE = t.cast(int, ...)
    TEXTURE_CUBE_MAP = t.cast(int, ...)
    TEXTURE_CUBE_MAP_ARRAY = t.cast(int, ...)
    TEXTURE_BUFFER = t.cast(int, ...)
    TEXTURE_2D_MULTISAMPLE = t.cast(int, ...)
    TEXTURE_2D_MULTISAMPLE_ARRAY = t.cast(int, ...)

class TextureParameter(enum.IntEnum):
    DEPTH_STENCIL_TEXTURE_MODE = t.cast(int, ...)
    TEXTURE_BASE_LEVEL = t.cast(int, ...)
    TEXTURE_BORDER_COLOR = t.cast(int, ...)
    TEXTURE_COMPARE_FUNC = t.cast(int, ...)
    TEXTURE_COMPARE_MODE = t.cast(int, ...)
    TEXTURE_LOD_BIAS = t.cast(int, ...)
    TEXTURE_MIN_FILTER = t.cast(int, ...)
    TEXTURE_MAG_FILTER = t.cast(int, ...)
    TEXTURE_MIN_LOD = t.cast(int, ...)
    TEXTURE_MAX_LOD = t.cast(int, ...)
    TEXTURE_MAX_LEVEL = t.cast(int, ...)
    TEXTURE_SWIZZLE_R = t.cast(int, ...)
    TEXTURE_SWIZZLE_G = t.cast(int, ...)
    TEXTURE_SWIZZLE_B = t.cast(int, ...)
    TEXTURE_SWIZZLE_A = t.cast(int, ...)
    TEXTURE_SWIZZLE_RGBA = t.cast(int, ...)
    TEXTURE_WRAP_S = t.cast(int, ...)
    TEXTURE_WRAP_T = t.cast(int, ...)
    TEXTURE_WRAP_R = t.cast(int, ...)

class MinFilter(enum.IntEnum):
    NEAREST = t.cast(int, ...)
    LINEAR = t.cast(int, ...)
    NEAREST_MIPMAP_NEAREST = t.cast(int, ...)
    LINEAR_MIPMAP_NEAREST = t.cast(int, ...)
    NEAREST_MIPMAP_LINEAR = t.cast(int, ...)
    LINEAR_MIPMAP_LINEAR = t.cast(int, ...)

class MagFilter(enum.IntEnum):
    NEAREST = t.cast(int, ...)
    LINEAR = t.cast(int, ...)

class WrapMode(enum.IntEnum):
    CLAMP_TO_EDGE = t.cast(int, ...)
    CLAMP_TO_BORDER = t.cast(int, ...)
    MIRROR_CLAMP_TO_EDGE = t.cast(int, ...)
    REPEAT = t.cast(int, ...)
    MIRRORED_REPEAT = t.cast(int, ...)

class InternalFormat(enum.IntEnum):
    R8 = t.cast(int, ...)
    R8_SNORM = t.cast(int, ...)
    R16 = t.cast(int, ...)
    R16_SNORM = t.cast(int, ...)
    RG8 = t.cast(int, ...)
    RG8_SNORM = t.cast(int, ...)
    RG16 = t.cast(int, ...)
    RG16_SNORM = t.cast(int, ...)
    R3_G3_B2 = t.cast(int, ...)
    RGB4 = t.cast(int, ...)
    RGB5 = t.cast(int, ...)
    RGB8 = t.cast(int, ...)
    RGB8_SNORM = t.cast(int, ...)
    RGB10 = t.cast(int, ...)
    RGB12 = t.cast(int, ...)
    RGB16 = t.cast(int, ...)
    RGB16_SNORM = t.cast(int, ...)
    RGBA2 = t.cast(int, ...)
    RGBA4 = t.cast(int, ...)
    RGB5_A1 = t.cast(int, ...)
    RGBA8 = t.cast(int, ...)
    RGBA8_SNORM = t.cast(int, ...)
    RGB10_A2 = t.cast(int, ...)
    RGB10_A2UI = t.cast(int, ...)
    RGBA12 = t.cast(int, ...)
    RGBA16 = t.cast(int, ...)
    SRGB8 = t.cast(int, ...)
    SRGB8_ALPHA8 = t.cast(int, ...)
    R16F = t.cast(int, ...)
    RG16F = t.cast(int, ...)
    RGB16F = t.cast(int, ...)
    RGBA16F = t.cast(int, ...)
    R32F = t.cast(int, ...)
    RG32F = t.cast(int, ...)
    RGB32F = t.cast(int, ...)
    RGBA32F = t.cast(int, ...)
    R11F_G11F_B10F = t.cast(int, ...)
    RGB9_E5 = t.cast(int, ...)
    R8I = t.cast(int, ...)
    R8UI = t.cast(int, ...)
    R16I = t.cast(int, ...)
    R16UI = t.cast(int, ...)
    R32I = t.cast(int, ...)
    R32UI = t.cast(int, ...)
    RG8I = t.cast(int, ...)
    RG8UI = t.cast(int, ...)
    RG16I = t.cast(int, ...)
    RG16UI = t.cast(int, ...)
    RG32I = t.cast(int, ...)
    RG32UI = t.cast(int, ...)
    RGB8I = t.cast(int, ...)
    RGB8UI = t.cast(int, ...)
    RGB16I = t.cast(int, ...)
    RGB16UI = t.cast(int, ...)
    RGB32I = t.cast(int, ...)
    RGB32UI = t.cast(int, ...)
    RGBA8I = t.cast(int, ...)
    RGBA8UI = t.cast(int, ...)
    RGBA16I = t.cast(int, ...)
    RGBA16UI = t.cast(int, ...)
    RGBA32I = t.cast(int, ...)
    RGBA32UI = t.cast(int, ...)

class CompressedInternalFormat(enum.IntEnum):
    COMPRESSED_RGB_S3TC_DXT1_EXT = t.cast(int, ...)
    COMPRESSED_RGBA_S3TC_DXT1_EXT = t.cast(int, ...)
    COMPRESSED_RGBA_S3TC_DXT3_EXT = t.cast(int, ...)
    COMPRESSED_RGBA_S3TC_DXT5_EXT = t.cast(int, ...)

class PixelFormat(enum.IntEnum):
    RED = t.cast(int, ...)
    RG = t.cast(int, ...)
    RGB = t.cast(int, ...)
    BGR = t.cast(int, ...)
    RGBA = t.cast(int, ...)
    BGRA = t.cast(int, ...)
    DEPTH_COMPONENT = t.cast(int, ...)
    STENCIL_INDEX = t.cast(int, ...)

class PixelType(enum.IntEnum):
    UNSIGNED_BYTE = t.cast(int, ...)
    BYTE = t.cast(int, ...)
    UNSIGNED_SHORT = t.cast(int, ...)
    SHORT = t.cast(int, ...)
    UNSIGNED_INT = t.cast(int, ...)
    INT = t.cast(int, ...)
    FLOAT = t.cast(int, ...)
    UNSIGNED_BYTE_3_3_2 = t.cast(int, ...)
    UNSIGNED_BYTE_2_3_3_REV = t.cast(int, ...)
    UNSIGNED_SHORT_5_6_5 = t.cast(int, ...)
    UNSIGNED_SHORT_5_6_5_REV = t.cast(int, ...)
    UNSIGNED_SHORT_4_4_4_4 = t.cast(int, ...)
    UNSIGNED_SHORT_4_4_4_4_REV = t.cast(int, ...)
    UNSIGNED_SHORT_5_5_5_1 = t.cast(int, ...)
    UNSIGNED_SHORT_1_5_5_5_REV = t.cast(int, ...)
    UNSIGNED_INT_8_8_8_8 = t.cast(int, ...)
    UNSIGNED_INT_8_8_8_8_REV = t.cast(int, ...)
    UNSIGNED_INT_10_10_10_2 = t.cast(int, ...)
    UNSIGNED_INT_2_10_10_10_REV = t.cast(int, ...)

class TextureSpec:
    target: int

    width: int
    height: int
    depth: int

    samples: int
    mipmaps: int

    internal_format: int

    min_filter: int
    mag_filter: int
    wrap_mode: int

    def __init__(self,
                 target: TextureTarget,
                 width: int,
                 height: int,
                 internal_format: InternalFormat | CompressedInternalFormat,
                 depth: int = 1,
                 samples: int = 1,
                 mipmaps: int = 1,
                 min_filter: MinFilter = MinFilter.LINEAR,
                 mag_filter: MagFilter = MagFilter.LINEAR,
                 wrap_mode: WrapMode = WrapMode.CLAMP_TO_EDGE) -> None: ...

    @property
    def size(self) -> tuple[int, int, int]: ...

    @property
    def is_multisampled(self) -> bool: ...

class UploadInfo:
    width: int
    height: int
    depth: int

    x_offset: int
    y_offset: int
    z_offset: int

    level: int
    format: int
    pixel_type: int

    data_offset: int
    image_size: int

    generate_mipmap: bool

    def __init__(self,
                 format: PixelFormat | CompressedInternalFormat,
                 width: int,
                 height: int,
                 depth: int = 1,
                 x_offset: int = 0,
                 y_offset: int = 0,
                 z_offset: int = 0,
                 level: int = 0,
                 pixel_type: PixelType = PixelType.UNSIGNED_BYTE,
                 image_size: int = 0,
                 data_offset: int = 0,
                 generate_mipmap: bool = True) -> None: ...

    @property
    def is_compressed(self) -> bool: ...

class Texture:
    def __init__(self, spec: TextureSpec) -> None: ...

    def delete(self) -> None: ...
    def bind(self) -> None: ...
    def bind_to_unit(self, unit: int) -> None: ...
    def upload(self, info: UploadInfo, data: TSupportsBuffer | None) -> None: ...
    def set_parameter(self, parameter: TextureParameter, value: int) -> None: ...
    def generate_mipmap(self) -> None: ...
    def set_texture_buffer(self, buffer: Buffer) -> None: ...

    @property
    def id(self) -> int: ...

    @property
    def target(self) -> TextureTarget: ...

    @property
    def internal_format(self) -> InternalFormat | CompressedInternalFormat: ...

    @property
    def width(self) -> int: ...

    @property
    def height(self) -> int: ...

    @property
    def depth(self) -> int: ...

    @property
    def mipmaps(self) -> int: ...

    @property
    def is_cubemap(self) -> bool: ...

    @property
    def is_array(self) -> bool: ...

    @property
    def is_1d(self) -> bool: ...

    @property
    def is_2d(self) -> bool: ...

    @property
    def is_3d(self) -> bool: ...

def bind_textures(textures: list[Texture], first: int = 0) -> None: ...
def bind_texture_ids(textures: list[int], first: int = 0) -> None: ...
def set_pixel_pack_alignment(alignment: t.Literal[1, 2, 4, 8]) -> None: ...
def set_pixel_unpack_alignment(alignment: t.Literal[1, 2, 4, 8]) -> None: ...
