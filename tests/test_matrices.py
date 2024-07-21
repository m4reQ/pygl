import numpy as np
import pytest

from pygl.math import Matrix4, Vector4


def test_matrix_init_no_args_success() -> None:
    m = Matrix4()

    assert m.row0 == Vector4.zero()
    assert m.row1 == Vector4.zero()
    assert m.row2 == Vector4.zero()
    assert m.row3 == Vector4.zero()

def test_matrix_init_identity_success() -> None:
    m = Matrix4.identity()

    assert m[0, 0] == 1.0
    assert m[1, 1] == 1.0
    assert m[2, 2] == 1.0
    assert m[3, 3] == 1.0

def test_matrix_init_float_success() -> None:
    m = Matrix4(3.0)

    assert m[0, 0] == 3.0
    assert m[1, 1] == 3.0
    assert m[2, 2] == 3.0
    assert m[3, 3] == 3.0

def test_matrix_init_buffer_success() -> None:
    data = np.zeros((4, 4), dtype=np.float32, order='C')
    data[0][0] = 1.0
    data[1][1] = 1.0
    data[2][2] = 1.0
    data[3][3] = 1.0

    m = Matrix4(data)

    assert m[0, 0] == 1.0
    assert m[1, 1] == 1.0
    assert m[2, 2] == 1.0
    assert m[3, 3] == 1.0

def test_matrix_init_one_arg_failure_invalid_type() -> None:
    with pytest.raises(TypeError):
        Matrix4('str')
