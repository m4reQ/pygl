import enum
import typing as t

class GLType(enum.IntEnum):
    BYTE = t.cast(int, ...)
    UNSIGNED_BYTE = t.cast(int, ...)
    SHORT = t.cast(int, ...)
    UNSIGNED_SHORT = t.cast(int, ...)
    INT = t.cast(int, ...)
    UNSIGNED_INT = t.cast(int, ...)
    FIXED = t.cast(int, ...)
    HALF_FLOAT = t.cast(int, ...)
    FLOAT = t.cast(int, ...)
    DOUBLE = t.cast(int, ...)

class BlendEquation(enum.IntEnum):
    FUNC_ADD = t.cast(int, ...)
    FUNC_SUBTRACT = t.cast(int, ...)
    FUNC_REVERSE_SUBTRACT = t.cast(int, ...)
    MIN = t.cast(int, ...)
    MAX = t.cast(int, ...)

class EnableCap(enum.IntEnum):
    BLEND = t.cast(int, ...)
    CLIP_DISTANCE0 = t.cast(int, ...)
    CLIP_DISTANCE1 = t.cast(int, ...)
    CLIP_DISTANCE2 = t.cast(int, ...)
    CLIP_DISTANCE3 = t.cast(int, ...)
    CLIP_DISTANCE4 = t.cast(int, ...)
    CLIP_DISTANCE5 = t.cast(int, ...)
    CLIP_DISTANCE6 = t.cast(int, ...)
    CLIP_DISTANCE7 = t.cast(int, ...)
    COLOR_LOGIC_OP = t.cast(int, ...)
    CULL_FACE = t.cast(int, ...)
    DEBUG_OUTPUT = t.cast(int, ...)
    DEBUG_OUTPUT_SYNCHRONOUS = t.cast(int, ...)
    DEPTH_CLAMP = t.cast(int, ...)
    DEPTH_TEST = t.cast(int, ...)
    DITHER = t.cast(int, ...)
    FRAMEBUFFER_SRGB = t.cast(int, ...)
    LINE_SMOOTH = t.cast(int, ...)
    MULTISAMPLE = t.cast(int, ...)
    POLYGON_OFFSET_FILL = t.cast(int, ...)
    POLYGON_OFFSET_LINE = t.cast(int, ...)
    POLYGON_OFFSET_POINT = t.cast(int, ...)
    POLYGON_SMOOTH = t.cast(int, ...)
    PRIMITIVE_RESTART = t.cast(int, ...)
    PRIMITIVE_RESTART_FIXED_INDEX = t.cast(int, ...)
    RASTERIZER_DISCARD = t.cast(int, ...)
    SAMPLE_ALPHA_TO_COVERAGE = t.cast(int, ...)
    SAMPLE_ALPHA_TO_ONE = t.cast(int, ...)
    SAMPLE_COVERAGE = t.cast(int, ...)
    SAMPLE_SHADING = t.cast(int, ...)
    SAMPLE_MASK = t.cast(int, ...)
    SCISSOR_TEST = t.cast(int, ...)
    STENCIL_TEST = t.cast(int, ...)
    TEXTURE_CUBE_MAP_SEAMLESS = t.cast(int, ...)
    PROGRAM_POINT_SIZE = t.cast(int, ...)

class HintTarget(enum.IntEnum):
    FRAGMENT_SHADER_DERIVATIVE_HINT = t.cast(int, ...)
    LINE_SMOOTH_HINT = t.cast(int, ...)
    POLYGON_SMOOTH_HINT = t.cast(int, ...)
    TEXTURE_COMPRESSION_HINT = t.cast(int, ...)

class HintValue(enum.IntEnum):
    NICEST = t.cast(int, ...)
    FASTEST = t.cast(int, ...)
    DONT_CARE = t.cast(int, ...)

class FrontFace(enum.IntEnum):
    CW = t.cast(int, ...)
    CCW = t.cast(int, ...)

class CullFace(enum.IntEnum):
    FRONT = t.cast(int, ...)
    BACK = t.cast(int, ...)
    FRONT_AND_BACK = t.cast(int, ...)

class DepthFunc(enum.IntEnum):
    NEVER = t.cast(int, ...)
    LESS = t.cast(int, ...)
    EQUAL = t.cast(int, ...)
    LEQUAL = t.cast(int, ...)
    GREATER = t.cast(int, ...)
    NOTEQUAL = t.cast(int, ...)
    GEQUAL = t.cast(int, ...)
    ALWAYS = t.cast(int, ...)

class PolygonMode(enum.IntEnum):
    FILL = t.cast(int, ...)
    POINT = t.cast(int, ...)
    LINE = t.cast(int, ...)

class BlendFactor(enum.IntEnum):
    ZERO = t.cast(int, ...)
    ONE = t.cast(int, ...)
    SRC_COLOR = t.cast(int, ...)
    ONE_MINUS_SRC_COLOR = t.cast(int, ...)
    DST_COLOR = t.cast(int, ...)
    ONE_MINUS_DST_COLOR = t.cast(int, ...)
    SRC_ALPHA = t.cast(int, ...)
    ONE_MINUS_SRC_ALPHA = t.cast(int, ...)
    DST_ALPHA = t.cast(int, ...)
    ONE_MINUS_DST_ALPHA = t.cast(int, ...)
    CONSTANT_COLOR = t.cast(int, ...)
    ONE_MINUS_CONSTANT_COLOR = t.cast(int, ...)
    CONSTANT_ALPHA = t.cast(int, ...)
    ONE_MINUS_CONSTANT_ALPHA = t.cast(int, ...)

class StringName:
    SHADING_LANGUAGE_VERSION: int
    VERSION: int
    RENDERER: int
    VENDOR: int
    EXTENSIONS: int

def cull_face(face: CullFace) -> None: ...
def front_face(face: FrontFace) -> None: ...
def hint(target: HintTarget, value: HintValue) -> None: ...
def enable(cap: EnableCap) -> None: ...
def disable(cap: EnableCap) -> None: ...
def point_size(point_size: float) -> None: ...
def depth_func(func: DepthFunc) -> None: ...
def polygon_mode(face: CullFace, mode: PolygonMode) -> None: ...
def blend_func(src: BlendFactor, dst: BlendFactor) -> None: ...
def blend_func_separate(src_rgb: BlendFactor, dst_rgb: BlendFactor, src_alpha: BlendFactor, dst_alpha: BlendFactor) -> None: ...
def blend_equation(equation: BlendEquation) -> None: ...
def get_string(name: StringName, index: int | None = None) -> None: ...
def clear_color(r: float, g: float, b: float, a: float) -> None: ...
def scissor(x: int, y: int, width: int, height: int) -> None: ...
def viewport(x: int, y: int, width: int, height: int) -> None: ...
def color_mask(r: bool, g: bool, b: bool, a: bool) -> None: ...
def depth_mask(enabled: bool) -> None: ...
