import enum

class BlendEquation(enum.IntEnum):
    FUNC_ADD: int
    FUNC_SUBTRACT: int
    FUNC_REVERSE_SUBTRACT: int
    MIN: int
    MAX: int

class EnableCap(enum.IntEnum):
    BLEND: int
    CLIP_DISTANCE0: int
    CLIP_DISTANCE1: int
    CLIP_DISTANCE2: int
    CLIP_DISTANCE3: int
    CLIP_DISTANCE4: int
    CLIP_DISTANCE5: int
    CLIP_DISTANCE6: int
    CLIP_DISTANCE7: int
    COLOR_LOGIC_OP: int
    CULL_FACE: int
    DEBUG_OUTPUT: int
    DEBUG_OUTPUT_SYNCHRONOUS: int
    DEPTH_CLAMP: int
    DEPTH_TEST: int
    DITHER: int
    FRAMEBUFFER_SRGB: int
    LINE_SMOOTH: int
    MULTISAMPLE: int
    POLYGON_OFFSET_FILL: int
    POLYGON_OFFSET_LINE: int
    POLYGON_OFFSET_POINT: int
    POLYGON_SMOOTH: int
    PRIMITIVE_RESTART: int
    PRIMITIVE_RESTART_FIXED_INDEX: int
    RASTERIZER_DISCARD: int
    SAMPLE_ALPHA_TO_COVERAGE: int
    SAMPLE_ALPHA_TO_ONE: int
    SAMPLE_COVERAGE: int
    SAMPLE_SHADING: int
    SAMPLE_MASK: int
    SCISSOR_TEST: int
    STENCIL_TEST: int
    TEXTURE_CUBE_MAP_SEAMLESS: int
    PROGRAM_POINT_SIZE: int

class HintTarget(enum.IntEnum):
    FRAGMENT_SHADER_DERIVATIVE_HINT: int
    LINE_SMOOTH_HINT: int
    POLYGON_SMOOTH_HINT: int
    TEXTURE_COMPRESSION_HINT: int

class HintValue(enum.IntEnum):
    NICEST: int
    FASTEST: int
    DONT_CARE: int

class FrontFace(enum.IntEnum):
    CW: int
    CCW: int

class CullFace(enum.IntEnum):
    FRONT: int
    BACK: int
    FRONT_AND_BACK: int

class DepthFunc(enum.IntEnum):
    NEVER: int
    LESS: int
    EQUAL: int
    LEQUAL: int
    GREATER: int
    NOTEQUAL: int
    GEQUAL: int
    ALWAYS: int

class PolygonMode(enum.IntEnum):
    FILL: int
    POINT: int
    LINE: int

class BlendFactor(enum.IntEnum):
    ZERO: int
    ONE: int
    SRC_COLOR: int
    ONE_MINUS_SRC_COLOR: int
    DST_COLOR: int
    ONE_MINUS_DST_COLOR: int
    SRC_ALPHA: int
    ONE_MINUS_SRC_ALPHA: int
    DST_ALPHA: int
    ONE_MINUS_DST_ALPHA: int
    CONSTANT_COLOR: int
    ONE_MINUS_CONSTANT_COLOR: int
    CONSTANT_ALPHA: int
    ONE_MINUS_CONSTANT_ALPHA: int

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
