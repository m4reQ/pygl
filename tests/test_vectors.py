import pytest

from pygl.math import (Vector2, Vector3, Vector4, get_closest_factors,
                       interpolate)


def test_vector_init_one_arg_success() -> None:
    v = Vector4(1.0)
    assert v.x == 1.0
    assert v.y == 1.0
    assert v.z == 1.0
    assert v.w == 1.0

def test_vector_init_multiple_args_success() -> None:
    v = Vector4(2.0, 1.0, 3.0, 7.0)
    assert v.x == 2.0
    assert v.y == 1.0
    assert v.z == 3.0
    assert v.w == 7.0

def test_vector_init_one_arg_failure_invalid_type() -> None:
    with pytest.raises(TypeError):
        Vector4('str')

def test_vector_init_multiple_args_failure_invalid_type() -> None:
    with pytest.raises(TypeError):
        Vector4(1.0, 2.0, '3.0', 4.0)

def test_vector_color_getters_success() -> None:
    v = Vector4(1.0)

    assert v.r == 1.0
    assert v.g == 1.0
    assert v.b == 1.0
    assert v.a == 1.0

def test_vector_one_constructor_success() -> None:
    v = Vector4.one()

    assert v.x == 1.0
    assert v.y == 1.0
    assert v.z == 1.0
    assert v.w == 1.0

def test_vector_zero_constructor_success() -> None:
    v = Vector4.zero()

    assert v.x == 0.0
    assert v.y == 0.0
    assert v.z == 0.0
    assert v.w == 0.0

def test_vector_len_success() -> None:
    v = Vector4(1.0)

    assert len(v) == 4

def test_vector_getitem_success() -> None:
    v = Vector4(1.0, 2.0, 3.0, 4.0)

    assert v[2] == 3.0

def test_vector_getitem_failure_index_out_of_bounds() -> None:
    v = Vector4(1.0)

    with pytest.raises(IndexError):
        v[4]

def test_vector_getitem_failure_index_invalid_type() -> None:
    v = Vector4(1.0)

    with pytest.raises(TypeError):
        v['3']

def test_vector_setitem_success() -> None:
    v = Vector4(1.0)
    v[1] = 3.5

    assert v.y == 3.5

def test_vector_setitem_failure_index_out_of_bounds() -> None:
    v = Vector4(1.0)

    with pytest.raises(IndexError):
        v[4] = 4.0

def test_vector_setitem_failure_index_invalid_type() -> None:
    v = Vector4(1.0)

    with pytest.raises(TypeError):
        v['4'] = 4.0

def test_vector_setitem_failure_value_invalid_type() -> None:
    v = Vector4(1.0)

    with pytest.raises(TypeError):
        v[2] = '2.0'

def test_vector_add_success() -> None:
    a = Vector4(1.0)
    b = Vector4(2.0)
    c = a + b

    assert c.x == 3.0
    assert c.y == 3.0
    assert c.z == 3.0
    assert c.w == 3.0

def test_vector_add_failure_invalid_operand_type() -> None:
    a = Vector4(1.0)
    b = 'str'

    with pytest.raises(TypeError):
        a + b

def test_vector_sub_success() -> None:
    a = Vector4(5.0)
    b = Vector4(3.0)
    c = a - b

    assert c.x == 2.0
    assert c.y == 2.0
    assert c.z == 2.0
    assert c.w == 2.0

def test_vector_normalize_success() -> None:
    v = Vector4(1.0)
    v.normalize()

    assert v.x == 0.5
    assert v.y == 0.5
    assert v.z == 0.5
    assert v.w == 0.5

def test_vector_normalized_success() -> None:
    v = Vector4(1.0)
    vn = v.normalized()

    assert vn.x == 0.5
    assert vn.y == 0.5
    assert vn.z == 0.5
    assert vn.w == 0.5

def test_vector_dot_success() -> None:
    a = Vector4(1.0)
    b = Vector4(2.0)
    dot = a.dot(b)

    assert dot == 8.0

def test_vector_dot_failure_invalid_type() -> None:
    a = Vector4(1.0)

    with pytest.raises(TypeError):
        a.dot('vec')

def test_vector3_cross_success() -> None:
    a = Vector3(1.0, 1.0, 0.0)
    b = Vector3(0.0, 1.0, 1.0)
    cross = a.cross(b)

    assert cross.x == 1.0
    assert cross.y == -1.0
    assert cross.z == 1.0

def test_vector2_cross_success() -> None:
    a = Vector2(1.0, 0.0)
    b = Vector2(0.0, 1.0)
    cross = a.cross(b)

    assert cross == 1.0

def test_vector_cross_failure_invalid_type() -> None:
    a = Vector3(1.0)
    b = 'str'

    with pytest.raises(TypeError):
        a.cross(b)

def test_vector_distance_success() -> None:
    a = Vector4(0.0)
    b = Vector4(1.0)
    dist = a.distance(b)

    assert dist == 2.0

def test_vector_distance_failure_invalid_type() -> None:
    a = Vector4(0.0)
    b = 'str'

    with pytest.raises(TypeError):
        a.distance(b)

def test_vector_interpolate_success() -> None:
    a = Vector4(0.0)
    b = Vector4(1.0)
    c = a.interpolate(b, 0.5)

    assert c.x == 0.5
    assert c.y == 0.5
    assert c.z == 0.5
    assert c.w == 0.5

def test_vector_interpolate_failure_invalid_value_type() -> None:
    a = Vector4(0.0)
    b = 'str'

    with pytest.raises(TypeError):
        a.interpolate(b, 0.5)

def test_vector_interpolate_failure_invalid_factor_type() -> None:
    a = Vector4(0.0)
    b = Vector4(1.0)

    with pytest.raises(TypeError):
        a.interpolate(b, 'str')

def test_vector_interpolate_failure_factor_too_low() -> None:
    a = Vector4(0.0)
    b = Vector4(1.0)

    with pytest.raises(ValueError):
        a.interpolate(b, -0.5)

def test_vector_interpolate_failure_factor_too_high() -> None:
    a = Vector4(0.0)
    b = Vector4(1.0)

    with pytest.raises(ValueError):
        a.interpolate(b, 1.5)
