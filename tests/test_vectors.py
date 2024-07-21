import pytest

from pygl.math import (Vector2, Vector3, Vector4, get_closest_factors,
                       interpolate)


def test_get_closest_factors_success() -> None:
    result = get_closest_factors(10)

    assert result == (2, 5)

def test_interpolate_float_success() -> None:
    result = interpolate(1.0, 2.0, 0.5)

    assert result == 1.5

def test_interpolate_vec2_success() -> None:
    result = interpolate(Vector2(1.0, 1.0), Vector2(2.0, 2.0), 0.5)

    assert isinstance(result, Vector2)
    assert result.x == 1.5
    assert result.y == 1.5

def test_interpolate_vec3_success() -> None:
    result = interpolate(Vector3(1.0, 1.0, 1.0), Vector3(2.0, 2.0, 2.0), 0.5)

    assert isinstance(result, Vector3)
    assert result.x == 1.5
    assert result.y == 1.5
    assert result.z == 1.5

def test_interpolate_vec4_success() -> None:
    result = interpolate(Vector4(2.0, 2.0, 2.0, 2.0), Vector4(4.0, 4.0, 4.0, 4.0), 0.5)

    assert isinstance(result, Vector4)
    assert result.x == 3.0
    assert result.y == 3.0
    assert result.z == 3.0
    assert result.w == 3.0
