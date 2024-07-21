import typing as t
from collections.abc import Buffer as TSupportsBuffer

class _Vector:
    @classmethod
    def one(cls) -> t.Self: ...

    @classmethod
    def zero(cls) -> t.Self: ...

    def __len__(self) -> int: ...
    def __getitem__(self, i: int) -> float: ...
    def __setitem__(self, v: float, i: int) -> None: ...
    def __add__(self, x: t.Self) -> t.Self: ...
    def __iadd__(self, x: t.Self) -> t.Self: ...
    def __sub__(self, x: t.Self) -> t.Self: ...
    def __isub__(self, x: t.Self) -> t.Self: ...

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

    def normalize(self) -> None: ...
    def normalized(self) -> Quaternion: ...
    def dot(self, other: Quaternion) -> Quaternion: ...
    def conjugate(self) -> None: ...
    def inverse(self) -> None: ...
    def inversed(self) -> Quaternion: ...
    def interpolate(self, other: Quaternion, factor: float) -> Quaternion: ...

    @property
    def angle(self) -> float: ...

    @property
    def axis(self) -> Vector3: ...

    @property
    def real(self) -> float: ...

    @property
    def imag(self) -> Vector3: ...

class Matrix[TVec](t.Protocol):
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
    def __imatmul__(self, other: t.Self) -> t.Self: ...

    @t.overload
    def __matmul__(self, other: t.Self) -> t.Self: ...

    @t.overload
    def __matmul__(self, other: TVec) -> TVec: ...

    def transpose(self) -> None: ...
    def transposed(self) -> t.Self: ...
    def inverse(self) -> None: ...
    def inversed(self) -> t.Self: ...

    @property
    def row0(self) -> TVec: ...

    @row0.setter
    def row0(self, value: TVec) -> None: ...

    @property
    def row1(self) -> TVec: ...

    @row1.setter
    def row1(self, value: TVec) -> None: ...

    @property
    def row2(self) -> TVec: ...

    @row2.setter
    def row2(self, value: TVec) -> None: ...

    @property
    def row3(self) -> TVec: ...

    @row3.setter
    def row3(self, value: TVec) -> None: ...

class Matrix2(Matrix[Vector2]):
    pass

class Matrix3(Matrix[Vector3]):
    pass

class Matrix4(Matrix[Vector4]):
    pass

TInterpolate = t.TypeVar('TInterpolate', Vector2, Vector3, Vector4, float, Quaternion)
def interpolate(x: TInterpolate, y: TInterpolate, factor: float) -> TInterpolate: ...
def get_closest_factors(x: float) -> float: ...
def rad_to_deg(a: float) -> float: ...
def deg_to_rad(a: float) -> float: ...