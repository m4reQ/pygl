import enum
import typing as t
from collections.abc import Buffer as TSupportsBuffer

from .buffers import Buffer

class TextureTarget(enum.IntEnum):
    TEXTURE_1D: int
    TEXTURE_2D: int
    TEXTURE_3D: int
    TEXTURE_1D_ARRAY: int
    TEXTURE_2D_ARRAY: int
    TEXTURE_RECTANGLE: int
    TEXTURE_CUBE_MAP: int
    TEXTURE_CUBE_MAP_ARRAY: int
    TEXTURE_BUFFER: int
    TEXTURE_2D_MULTISAMPLE: int
    TEXTURE_2D_MULTISAMPLE_ARRAY: int

class TextureParameter(enum.IntEnum):
    DEPTH_STENCIL_TEXTURE_MODE: int
    TEXTURE_BASE_LEVEL: int
    TEXTURE_BORDER_COLOR: int
    TEXTURE_COMPARE_FUNC: int
    TEXTURE_COMPARE_MODE: int
    TEXTURE_LOD_BIAS: int
    TEXTURE_MIN_FILTER: int
    TEXTURE_MAG_FILTER: int
    TEXTURE_MIN_LOD: int
    TEXTURE_MAX_LOD: int
    TEXTURE_MAX_LEVEL: int
    TEXTURE_SWIZZLE_R: int
    TEXTURE_SWIZZLE_G: int
    TEXTURE_SWIZZLE_B: int
    TEXTURE_SWIZZLE_A: int
    TEXTURE_SWIZZLE_RGBA: int
    TEXTURE_WRAP_S: int
    TEXTURE_WRAP_T: int
    TEXTURE_WRAP_R: int

class MinFilter(enum.IntEnum):
    NEAREST: int
    LINEAR: int
    NEAREST_MIPMAP_NEAREST: int
    LINEAR_MIPMAP_NEAREST: int
    NEAREST_MIPMAP_LINEAR: int
    LINEAR_MIPMAP_LINEAR: int

class MagFilter(enum.IntEnum):
    NEAREST: int
    LINEAR: int

class WrapMode(enum.IntEnum):
    CLAMP_TO_EDGE: int
    CLAMP_TO_BORDER: int
    MIRROR_CLAMP_TO_EDGE: int
    REPEAT: int
    MIRRORED_REPEAT: int

class InternalFormat(enum.IntEnum):
    R8: int
    R8_SNORM: int
    R16: int
    R16_SNORM: int
    RG8: int
    RG8_SNORM: int
    RG16: int
    RG16_SNORM: int
    R3_G3_B2: int
    RGB4: int
    RGB5: int
    RGB8: int
    RGB8_SNORM: int
    RGB10: int
    RGB12: int
    RGB16_SNORM: int
    RGBA2: int
    RGBA4: int
    RGB5_A1: int
    RGBA8: int
    RGBA8_SNORM: int
    RGB10_A2: int
    RGB10_A2UI: int
    RGBA12: int
    RGBA16: int
    SRGB8: int
    SRGB8_ALPHA8: int
    R16F: int
    RG16F: int
    RGB16F: int
    RGBA16F: int
    R32F: int
    RG32F: int
    RGB32F: int
    RGBA32F: int
    R11F_G11F_B10F: int
    RGB9_E5: int
    R8I: int
    R8UI: int
    R16I: int
    R16UI: int
    R32I: int
    R32UI: int
    RG8I: int
    RG8UI: int
    RG16I: int
    RG16UI: int
    RG32I: int
    RG32UI: int
    RGB8I: int
    RGB8UI: int
    RGB16I: int
    RGB16UI: int
    RGB32I: int
    RGB32UI: int
    RGBA8I: int
    RGBA8UI: int
    RGBA16I: int
    RGBA16UI: int
    RGBA32I: int
    RGBA32UI: int

class CompressedInternalFormat(enum.IntEnum):
    COMPRESSED_RGB_S3TC_DXT1_EXT: int
    COMPRESSED_RGBA_S3TC_DXT1_EXT: int
    COMPRESSED_RGBA_S3TC_DXT3_EXT: int
    COMPRESSED_RGBA_S3TC_DXT5_EXT: int

class PixelFormat(enum.IntEnum):
    RED: int
    RG: int
    RGB: int
    BGR: int
    RGBA: int
    BGRA: int
    DEPTH_COMPONENT: int
    STENCIL_INDEX: int

class PixelType(enum.IntEnum):
    UNSIGNED_BYTE: int
    BYTE: int
    UNSIGNED_SHORT: int
    SHORT: int
    UNSIGNED_INT: int
    INT: int
    FLOAT: int
    UNSIGNED_BYTE_3_3_2: int
    UNSIGNED_BYTE_2_3_3_REV: int
    UNSIGNED_SHORT_5_6_5: int
    UNSIGNED_SHORT_5_6_5_REV: int
    UNSIGNED_SHORT_4_4_4_4: int
    UNSIGNED_SHORT_4_4_4_4_REV: int
    UNSIGNED_SHORT_5_5_5_1: int
    UNSIGNED_SHORT_1_5_5_5_REV: int
    UNSIGNED_INT_8_8_8_8: int
    UNSIGNED_INT_8_8_8_8_REV: int
    UNSIGNED_INT_10_10_10_2: int
    UNSIGNED_INT_2_10_10_10_REV: int

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
    def upload(self, info: UploadInfo, data: TSupportsBuffer) -> None: ...
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
