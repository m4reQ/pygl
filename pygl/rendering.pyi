import enum

class Barrier(enum.IntEnum):
    VERTEX_ATTRIB_ARRAY_BARRIER_BIT: int
    ELEMENT_ARRAY_BARRIER_BIT: int
    UNIFORM_BARRIER_BIT: int
    TEXTURE_FETCH_BARRIER_BIT: int
    SHADER_IMAGE_ACCESS_BARRIER_BIT: int
    COMMAND_BARRIER_BIT: int
    PIXEL_BUFFER_BARRIER_BIT: int
    TEXTURE_UPDATE_BARRIER_BIT: int
    BUFFER_UPDATE_BARRIER_BIT: int
    CLIENT_MAPPED_BUFFER_BARRIER_BIT: int
    FRAMEBUFFER_BARRIER_BIT: int
    TRANSFORM_FEEDBACK_BARRIER_BIT: int
    ATOMIC_COUNTER_BARRIER_BIT: int
    SHADER_STORAGE_BARRIER_BIT: int
    QUERY_BUFFER_BARRIER_BIT: int
    ALL_BARRIER_BITS: int

class DrawMode(enum.IntEnum):
    POINTS: int
    LINE_STRIP: int
    LINE_LOOP: int
    LINES: int
    LINE_STRIP_ADJACENCY: int
    LINES_ADJACENCY: int
    TRIANGLE_STRIP: int
    TRIANGLE_FAN: int
    TRIANGLES: int
    TRIANGLE_STRIP_ADJACENCY: int
    TRIANGLES_ADJACENCY: int
    PATCHES: int

class ElementsType(enum.IntEnum):
    UNSIGNED_BYTE: int
    UNSIGNED_SHORT: int
    UNSIGNED_INT: int

class ClearMask(enum.IntFlag):
    COLOR_BUFFER_BIT: int
    DEPTH_BUFFER_BIT: int
    STENCIL_BUFFER_BIT: int

def draw_arrays(mode: DrawMode, first: int, count: int) -> None: ...
def draw_arrays_instanced(mode: DrawMode, first: int, count: int, instance_count: int) -> None: ...
def draw_arrays_instanced_base_instance(mode: DrawMode, first: int, count: int, instance_count: int, base_count: int) -> None: ...
def draw_arrays_indirect(mode: DrawMode) -> None: ...
def multi_draw_arrays_indirect(mode: DrawMode, draw_count: int, stride: int) -> None: ...

def draw_elements(mode: DrawMode, count: int, type: ElementsType, offset: int = 0) -> None: ...
def draw_elements_base_vertex(mode: DrawMode, count: int, type: ElementsType, base_vertex: int, offset: int = 0) -> None: ...
def draw_elements_instanced(mode: DrawMode, count: int, type: ElementsType, prim_count: int) -> None: ...
def draw_elements_instanced_base_instance(mode: DrawMode, count: int, type: ElementsType, instance_count: int, base_instance: int) -> None: ...
def draw_elements_instanced_base_vertex(mode: DrawMode, count: int, type: ElementsType, instance_count: int, base_vertex: int) -> None: ...
def draw_elements_instanced_base_vertex_base_instance(mode: DrawMode, count: int, type: ElementsType, instance_count: int, base_vertex: int, base_instance: int) -> None: ...
def draw_elements_indirect(mode: DrawMode, type: ElementsType) -> None: ...
def multi_draw_elements_indirect(mode: DrawMode, type: ElementsType, draw_count: int, stride: int) -> None: ...

def clear(mask: ClearMask) -> None: ...

def memory_barrier(barrier: Barrier) -> None: ...
def memory_barrier_by_region(barrier: Barrier) -> None: ...

def flush() -> None: ...
def finish() -> None: ...
