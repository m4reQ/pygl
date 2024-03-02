import enum

from _types import TSupportsBuffer as TSupportsBuffer

class ShaderType(enum.IntEnum):
    FRAGMENT_SHADER: int
    VERTEX_SHADER: int
    GEOMETRY_SHADER: int
    COMPUTE_SHADER: int
    TESS_CONTROL_SHADER: int
    TESS_EVALUATION_SHADER: int

class UniformType(enum.IntEnum):
    FLOAT: int
    DOUBLE: int
    INT: int
    UNSIGNED_INT: int

class ShaderStage:
    type: int
    filepath: str

    def __init__(self, type: ShaderType, filepath: str) -> None: ...

class Shader:
    def __init__(self, stages: list[ShaderStage]) -> None: ...

    @property
    def id(self) -> int: ...

    @property
    def resources(self) -> dict[str, int]: ...

    def use(self) -> None: ...
    def delete(self) -> None: ...
    def get_resource_location(self, name: str) -> int: ...
    def validate(self) -> None: ...
    def set_uniform_block_binding(self, name: str, binding: int) -> None: ...
    def get_uniform_block_location(self, name: str) -> int: ...
    def set_uniform(self, name: str, value: int | float) -> None: ...
    def set_uniform_array(self, name: str, value: TSupportsBuffer, type: UniformType) -> None: ...
