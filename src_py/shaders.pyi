import enum
from collections.abc import Buffer as TSupportsBuffer

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
    from_source: bool

    def __init__(self, type: ShaderType, filepath: str, from_source: bool = False) -> None: ...

TUValue = int | float

class Shader:
    def __init__(self, stages: list[ShaderStage]) -> None: ...

    @property
    def id(self) -> int: ...

    @property
    def resources(self) -> dict[str, int]: ...

    def use(self) -> None: ...
    def delete(self) -> None: ...
    def validate(self) -> None: ...

    def get_resource_location(self, name: str) -> int: ...
    def set_uniform_block_binding(self, name: str, binding: int) -> None: ...
    def get_uniform_block_location(self, name: str) -> int: ...

    def set_uniform(self, name: str, value: TUValue, type: UniformType = UniformType.FLOAT) -> None: ...
    def set_uniform_array(self, name: str, value: TSupportsBuffer, type: UniformType = UniformType.FLOAT) -> None: ...

    def set_uniform_vec2(self, name: str, x: TUValue, y: TUValue, type: UniformType = UniformType.FLOAT) -> None: ...
    def set_uniform_vec3(self, name: str, x: TUValue, y: TUValue, z: TUValue, type: UniformType = UniformType.FLOAT) -> None: ...
    def set_uniform_vec4(self, name: str, x: TUValue, y: TUValue, z: TUValue, w: TUValue, type: UniformType = UniformType.FLOAT) -> None: ...

    def set_uniform_mat2(self, name: str, value: TSupportsBuffer, type: UniformType = UniformType.FLOAT, transpose: bool = False) -> None: ...
    def set_uniform_mat3(self, name: str, value: TSupportsBuffer, type: UniformType = UniformType.FLOAT, transpose: bool = False) -> None: ...
    def set_uniform_mat4(self, name: str, value: TSupportsBuffer, type: UniformType = UniformType.FLOAT, transpose: bool = False) -> None: ...
