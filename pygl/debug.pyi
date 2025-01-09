import enum
import typing as t

from .buffers import Buffer
from .framebuffer import Framebuffer
from .shaders import Shader
from .sync import Sync
from .textures import Texture
from .vertex_array import VertexArray

class DebugSeverity(enum.IntEnum):
    DEBUG_SEVERITY_NOTIFICATION: int
    DEBUG_SEVERITY_LOW: int
    DEBUG_SEVERITY_MEDIUM: int
    DEBUG_SEVERITY_HIGH: int

class DebugSource(enum.IntEnum):
    DEBUG_SOURCE_API: int
    DEBUG_SOURCE_APPLICATION: int
    DEBUG_SOURCE_THIRD_PARTY: int
    DEBUG_SOURCE_WINDOW_SYSTEM: int
    DEBUG_SOURCE_OTHER: int
    DEBUG_SOURCE_SHADER_COMPILER: int

class DebugType(enum.IntEnum):
    DEBUG_TYPE_DEPRECATED_BEHAVIOR: int
    DEBUG_TYPE_MARKER: int
    DEBUG_TYPE_OTHER: int
    DEBUG_TYPE_UNDEFINED_BEHAVIOR: int
    DEBUG_TYPE_PORTABILITY: int
    DEBUG_TYPE_POP_GROUP: int
    DEBUG_TYPE_PUSH_GROUP: int
    DEBUG_TYPE_ERROR: int
    DEBUG_TYPE_PERFORMANCE: int

class ErrorCode(enum.IntEnum):
    NO_ERROR: int
    INVALID_ENUM: int
    INVALID_VALUE: int
    INVALID_OPERATION: int
    INVALID_FRAMEBUFFER_OPERATION: int
    OUT_OF_MEMORY: int
    STACK_UNDERFLOW: int
    STACK_OVERFLOW: int

class LogLevel(enum.IntEnum):
    INFO: int
    WARNING: int
    ERROR: int

TDebugCallback = t.Callable[[int, int, int, int, str, t.Any], t.Any]
TLogCallback = t.Callable[[int, str], None]
TGLObject = Texture | Buffer | Shader | Framebuffer | VertexArray

def enable(callback: TDebugCallback, user_data: t.Any = None) -> None: ...
def disable() -> None: ...
def get_error() -> int: ...
def insert_message(source: DebugSource, type: DebugType, id: int, severity: DebugSeverity, msg: str) -> None: ...
def set_object_name(obj: TGLObject | Sync, name: str) -> None: ...
def set_log_callback(callback: TLogCallback) -> None: ...
