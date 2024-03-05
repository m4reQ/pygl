import enum
import typing as t

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

TDebugCallback = t.Callable[[int, int, int, int, str, t.Any], t.Any]

def enable(callback: TDebugCallback, user_data: t.Any = None) -> None: ...
def disable() -> None: ...
def get_error() -> int: ...
