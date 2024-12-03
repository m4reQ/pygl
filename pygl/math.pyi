import typing as t
from collections.abc import Buffer as TSupportsBuffer

class _Vector:
    @classmethod
    def length(cls) -> int: ...

    @classmethod
    def one(cls) -> t.Self: ...

    @classmethod
    def zero(cls) -> t.Self: ...

    def __len__(self) -> int: ...
    def __getitem__(self, i: int) -> float: ...
    def __setitem__(self, v: float, i: int) -> None: ...

    def __add__(self, other: t.Self) -> t.Self: ...
    def __iadd__(self, other: t.Self) -> t.Self: ...
    def __sub__(self, other: t.Self) -> t.Self: ...
    def __isub__(self, other: t.Self) -> t.Self: ...
    def __mul__(self, other: t.Self | float) -> t.Self: ...
    def __imul__(self, other: t.Self | float) -> t.Self: ...
    def __neg__(self) -> t.Self: ...

    def __eq__(self, other: t.Self) -> bool: ...
    def __ne__(self, other: t.Self) -> bool: ...

    def normalize(self) -> None: ...
    def normalized(self) -> t.Self: ...
    def dot(self, other: t.Self) -> float: ...
    def distance(self, other: t.Self) -> float: ...
    def interpolate(self, other: t.Self, factor: float) -> t.Self: ...

class Vector2(_Vector):
    x: float
    y: float

    @t.overload
    def __init__(self, value: float) -> None: ...

    @t.overload
    def __init__(self, x: float, y: float) -> None: ...

    def cross(self, other: t.Self) -> float: ...

    @property
    def r(self) -> float: ...

    @property
    def g(self) -> float: ...

class Vector3(_Vector):
    x: float
    y: float
    z: float

    @classmethod
    def forward(cls) -> t.Self: ...

    @classmethod
    def up(cls) -> t.Self: ...

    @classmethod
    def right(cls) -> t.Self: ...

    @t.overload
    def __init__(self, value: float) -> None: ...

    @t.overload
    def __init__(self, x: float, y: float, z: float) -> None: ...

    def cross(self, other: t.Self) -> t.Self: ...

    @property
    def r(self) -> float: ...

    @property
    def g(self) -> float: ...

    @property
    def b(self) -> float: ...

class Vector4(_Vector):
    x: float
    y: float
    z: float
    w: float

    @t.overload
    def __init__(self, value: float) -> None: ...

    @t.overload
    def __init__(self, x: float, y: float, z: float, w: float) -> None: ...

    @property
    def r(self) -> float: ...

    @property
    def g(self) -> float: ...

    @property
    def b(self) -> float: ...

    @property
    def a(self) -> float: ...

class Quaternion:
    x: float
    y: float
    z: float
    w: float

    @classmethod
    def identity(cls) -> t.Self: ...

    @classmethod
    def from_axis(cls, angle: float, axis: Vector3) -> t.Self: ...

    @classmethod
    def look_at(cls, dir: Vector3, up: Vector3) -> t.Self: ...

    def __init__(self, x: float, y: float, z: float, w: float) -> None: ...
    def __mul__(self, other: Quaternion) -> Quaternion: ...
    def __imul__(self, other: Quaternion) -> Quaternion: ...

    def normalize(self) -> None: ...
    def normalized(self) -> Quaternion: ...
    def dot(self, other: Quaternion) -> Quaternion: ...
    def conjugate(self) -> None: ...
    def inverse(self) -> None: ...
    def inversed(self) -> Quaternion: ...
    def interpolate(self, other: Quaternion, factor: float) -> Quaternion: ...
    def rotate_vector(self, vector: Vector3) -> Vector3: ...

    @property
    def angle(self) -> float: ...

    @property
    def axis(self) -> Vector3: ...

    @property
    def real(self) -> float: ...

    @property
    def imag(self) -> Vector3: ...

class Plane:
    normal_x: float
    normal_y: float
    normal_z: float
    distance: float

    def __init__(self, normal: Vector3, distance: float) -> None: ...

    @property
    def normal(self) -> Vector3: ...

TVec = t.TypeVar('TVec', bound=_Vector)

class _Matrix[TVec]:
    @classmethod
    def length(cls) -> int: ...

    @classmethod
    def identity(cls) -> t.Self: ...

    @t.overload
    def __init__(self) -> None: ...

    @t.overload
    def __init__(self, value: float) -> None: ...

    @t.overload
    def __init__(self, value: TSupportsBuffer) -> None: ...

    @t.overload
    def __init__(self, row0: TVec, row1: TVec, row2: TVec, row3: TVec) -> None: ...

    @t.overload
    def __init__(self, values: list[float]) -> None: ...

    def __getitem__(self, indices: tuple[int, int]) -> float: ...
    def __setitem__(self, indices: tuple[int, int], value: float) -> None: ...
    def __add__(self, other: t.Self) -> t.Self: ...
    def __iadd__(self, other: t.Self) -> t.Self: ...
    def __sub__(self, other: t.Self) -> t.Self: ...
    def __isub__(self, other: t.Self) -> t.Self: ...
    def __imatmul__(self, other: t.Self) -> t.Self: ...

    @t.overload
    def __matmul__(self, other: t.Self) -> t.Self: ...

    @t.overload
    def __matmul__(self, other: TVec) -> TVec: ...

    def __len__(self) -> int: ...

    def transpose(self) -> None: ...
    def transposed(self) -> t.Self: ...
    def inverse(self) -> None: ...
    def inversed(self) -> t.Self: ...

    @property
    def row0(self) -> TVec: ...

    @property
    def row1(self) -> TVec: ...

    @property
    def row2(self) -> TVec: ...

    @property
    def row3(self) -> TVec: ...

class Matrix2(_Matrix[Vector2]):
    pass

class Matrix3(_Matrix[Vector3]):
    pass

class Matrix4(_Matrix[Vector4]):
    @classmethod
    def perspective(cls,
                    fov: float,
                    aspect: float,
                    z_near: float = -1.0,
                    z_far: float = 1.0) -> t.Self: ...

    @classmethod
    def ortho(cls,
              left: float,
              right: float,
              bottom: float,
              top: float,
              z_near: float = -1.0,
              z_far: float = 1.0) -> t.Self: ...

    @classmethod
    def look_at(cls, eye: Vector3, center: Vector3, up: Vector3 = Vector3(0.0, 1.0, 0.0)) -> t.Self: ...

    @t.overload
    @classmethod
    def look(cls, eye: Vector3, orientation: Quaternion) -> t.Self: ...

    @t.overload
    @classmethod
    def look(cls, eye: Vector3, dir: Vector3, up: Vector3 = Vector3(0.0, 1.0, 0.0)) -> t.Self: ...

    @t.overload
    @classmethod
    def transform(cls, translation: Vector3) -> t.Self: ...

    @t.overload
    @classmethod
    def transform(cls, translation: Vector3, scale: Vector3) -> t.Self: ...

    @t.overload
    @classmethod
    def transform(cls, translation: Vector3, scale: Vector3, rotation: Quaternion) -> t.Self: ...

TInterpolate = t.TypeVar('TInterpolate', Vector2, Vector3, Vector4, float, Quaternion)
def interpolate(x: TInterpolate, y: TInterpolate, factor: float) -> TInterpolate: ...
def get_closest_factors(x: float) -> float: ...
def rad_to_deg(a: float) -> float: ...
def deg_to_rad(a: float) -> float: ...