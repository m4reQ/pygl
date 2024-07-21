#pragma once

#ifndef MAT_LEN
#error "Matrix length not defined (MAT_LEN)"
#endif

#include <stdbool.h>
#include "matrix.h"
#include "../vector/vector.h"
#include "../../utility.h"

#define _TYPE CAT(Matrix, MAT_LEN)
#define _PY_TYPE CAT(CAT(pyMatrix, MAT_LEN), Type)
#define _GLM_TYPE CAT(mat, MAT_LEN)
#define _VEC_TYPE CAT(Vector, MAT_LEN)
#define _VEC_PY_TYPE CAT(CAT(pyVector, MAT_LEN), Type)
#define _SELF _TYPE *self
#define _GLM_INVOKE(func, ...) CAT(CAT(CAT(glm_mat, MAT_LEN), _), func)(__VA_ARGS__)
#define _NEW(var) _TYPE *var = PyObject_New(_TYPE, &_PY_TYPE)
#define _GLM_VEC_COPY(src, dst) CAT(CAT(glm_vec, MAT_LEN), _copy)(src, dst)

static int init(_SELF, PyObject *args, PyObject *kwargs)
{
    size_t argsLen = PyTuple_GET_SIZE(args);

    if (argsLen == 0)
    {
        _GLM_INVOKE(zero, self->data);
    }
    else if (argsLen == 1) // Matrix*.__init__(float | Buffer | list[float])
    {
        PyObject *arg = PyTuple_GET_ITEM(args, 0);

        if (PyFloat_Check(arg))
        {
            float diag = PyFloat_AS_DOUBLE(arg);

            _GLM_INVOKE(zero, self->data);

            for (size_t i = 0; i < MAT_LEN; i++)
                self->data[i][i] = diag;
        }
        else if (PyObject_CheckBuffer(arg))
        {
            Py_buffer buf;
            if (!PyObject_GetBuffer(arg, &buf, PyBUF_C_CONTIGUOUS))
            {
                raise_buffer_not_contiguous();
                return -1;
            }

            PyBuffer_ToContiguous(self->data, &buf, sizeof(_GLM_TYPE), 'C');
            PyBuffer_Release(&buf);
        }
        else if (PyList_Check(arg))
        {
            PyObject *seq = PySequence_Fast(arg, NULL);

            size_t valuesCount = PySequence_Fast_GET_SIZE(seq);
            if (valuesCount != MAT_LEN * MAT_LEN)
            {
                PyErr_Format(PyExc_ValueError, "List of matrix values must contain %d elements.", MAT_LEN * MAT_LEN);
                return -1;
            }

            PyObject **values = PySequence_Fast_ITEMS(seq);
            for (size_t i = 0; i < valuesCount; i++)
            {
                ((float *)self->data)[i] = PyFloat_AsDouble(values[i]);
                if (PyErr_Occurred())
                    return -1;
            }
        }
        else
        {
            PyErr_Format(PyExc_TypeError, "Expected argument to be of type float or support buffer protocol, got %s.", Py_TYPE(arg)->tp_name);
            return -1;
        }
    }
    else if (argsLen == MAT_LEN) // Matrix*.__init__(Vector*, Vector*, Vector*, Vector*)
    {
        for (size_t i = 0; i < MAT_LEN; i++)
        {
            PyObject *row = PyTuple_GET_ITEM(args, 0);
            if (!PyObject_IsInstance(row, (PyObject *)&_VEC_PY_TYPE))
            {
                PyErr_Format(
                    PyExc_TypeError,
                    "Expected all arguments to be of type pygl.math.Vector" STRINGIFY(MAT_LEN) ", got %s at index %d.",
                    Py_TYPE(row)->tp_name,
                    i);
                return -1;
            }

            void *rowData = ((_VEC_TYPE *)row)->data;
            _GLM_VEC_COPY(rowData, self->data[i]);
        }
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments provided. See usage of Matrix" STRINGIFY(MAT_LEN) ".__init__."); // TODO Add link to documentation
        return -1;
    }

    return 0;
}

static PyObject *identity(PyTypeObject *cls, PyObject *Py_UNUSED(args))
{
    _TYPE *res = PyObject_New(_TYPE, cls);
    _GLM_INVOKE(identity, res->data);

    return (PyObject *)res;
}

static PyObject *transpose(_SELF, PyObject *Py_UNUSED(args))
{
    _GLM_INVOKE(transpose, self->data);
    Py_RETURN_NONE;
}

static PyObject *transposed(_SELF, PyObject *Py_UNUSED(args))
{
    _NEW(res);
    _GLM_INVOKE(transpose_to, self->data, res->data);
    return (PyObject *)res;
}

static PyObject *inverse(_SELF, PyObject *Py_UNUSED(args))
{
    _GLM_TYPE res;
    _GLM_INVOKE(inv, self->data, res);
    _GLM_INVOKE(copy, res, self->data);

    Py_RETURN_NONE;
}

static PyObject *inversed(_SELF, PyObject *Py_UNUSED(args))
{
    _NEW(res);
    _GLM_INVOKE(inv, self->data, res->data);
    return (PyObject *)res;
}

static bool row_set(_SELF, PyObject *value, size_t rowIdx)
{
    assert(rowIdx < MAT_LEN);

    if (!PyObject_IsInstance(value, (PyObject *)&_VEC_PY_TYPE))
    {
        PyErr_SetString(PyExc_TypeError, "Value has to be of type pygl.math.Vector" STRINGIFY(MAT_LEN) ".");
        return false;
    }

    void *rowData = ((_VEC_TYPE *)value)->data;
    _GLM_VEC_COPY(rowData, self->data[rowIdx]);

    return true;
}

static PyObject *row_get(_SELF, size_t rowIdx)
{
    assert(rowIdx < MAT_LEN);

    _VEC_TYPE *res = PyObject_New(_VEC_TYPE, &_VEC_PY_TYPE);
    _GLM_VEC_COPY(self->data[rowIdx], res->data);

    return (PyObject *)res;
}

static PyObject *row0_get(_SELF, void *Py_UNUSED(closure))
{
    return row_get(self, 0);
}

static int row0_set(_SELF, PyObject *value, void *Py_UNUSED(closure))
{
    return row_set(self, value, 0) ? 0 : -1;
}

static PyObject *row1_get(_SELF, void *Py_UNUSED(closure))
{
    return row_get(self, 1);
}

static int row1_set(_SELF, PyObject *value, void *Py_UNUSED(closure))
{
    return row_set(self, value, 1) ? 0 : -1;
}

static PyObject *row2_get(_SELF, void *Py_UNUSED(closure))
{
    return row_get(self, 2);
}

static int row2_set(_SELF, PyObject *value, void *Py_UNUSED(closure))
{
    return row_set(self, value, 2) ? 0 : -1;
}

static PyObject *row3_get(_SELF, void *Py_UNUSED(closure))
{
    return row_get(self, 3);
}

static int row3_set(_SELF, PyObject *value, void *Py_UNUSED(closure))
{
    return row_set(self, value, 3) ? 0 : -1;
}

#pragma region AS_NUMBER
static PyObject *matmul(_SELF, PyObject *other)
{
    if (PyObject_IsInstance(other, (PyObject *)&_PY_TYPE))
    {
        _NEW(res);
        _GLM_INVOKE(mul, self->data, ((_TYPE *)other)->data, res->data);

        return (PyObject *)res;
    }
    else if (PyObject_IsInstance(other, (PyObject *)&_VEC_PY_TYPE))
    {
        _VEC_TYPE *res = PyObject_New(_VEC_TYPE, &_VEC_PY_TYPE);
        _GLM_INVOKE(mulv, self->data, ((_VEC_TYPE *)other)->data, res->data);

        return (PyObject *)res;
    }

    Py_RETURN_NOTIMPLEMENTED;
}

static PyObject *imatmul(_SELF, PyObject *other)
{
    if (!PyObject_IsInstance(other, (PyObject *)&_PY_TYPE))
        Py_RETURN_NOTIMPLEMENTED;

    _GLM_TYPE res;
    _GLM_INVOKE(mul, self->data, ((_TYPE *)other)->data, res);
    _GLM_INVOKE(copy, res, self->data);

    return (PyObject *)self;
}
#pragma endregion

#pragma region AS_MAPPING
static bool get_indices(PyObject *indices, unsigned int *n, unsigned int *m)
{
    if (!PyTuple_Check(indices) || PyTuple_GET_SIZE(indices) != 2)
    {
        PyErr_SetString(PyExc_TypeError, "Only indices in form of (n, m) tuple are allowed.");
        return false;
    }

    PyObject *_n = PyTuple_GET_ITEM(indices, 0);
    PyObject *_m = PyTuple_GET_ITEM(indices, 1);

    if (!(PyLong_Check(_n) && PyLong_Check(_m)))
    {
        PyErr_SetString(PyExc_ValueError, "Invalid indices provided: must be a tuple of two integers.");
        return false;
    }

    *n = PyLong_AsUnsignedLong(_n);
    *m = PyLong_AsUnsignedLong(_m);

    return true;
}

static Py_ssize_t len(_SELF)
{
    return 16;
}

static PyObject *subscript(_SELF, PyObject *indices)
{
    unsigned int n;
    unsigned int m;
    if (!get_indices(indices, &n, &m))
        return NULL;

    return PyFloat_FromDouble(self->data[n][m]);
}

static int subscript_assign(_SELF, PyObject *indices, PyObject *value)
{
    if (!PyFloat_Check(value))
    {
        PyErr_Format(PyExc_TypeError, "Item must be of type float, got %s.", Py_TYPE(value)->tp_name);
        return -1;
    }

    unsigned int n;
    unsigned int m;
    if (!get_indices(indices, &n, &m))
        return -1;

    self->data[n][m] = PyFloat_AS_DOUBLE(value);

    return 0;
}
#pragma endregion

PyTypeObject _PY_TYPE = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_new = PyType_GenericNew,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_basicsize = sizeof(_TYPE),
    .tp_name = "pygl.math.Matrix" STRINGIFY(MAT_LEN),
    .tp_as_number = &(PyNumberMethods){
        .nb_matrix_multiply = (binaryfunc)matmul,
        .nb_inplace_matrix_multiply = (binaryfunc)imatmul,
    },
    .tp_as_mapping = &(PyMappingMethods){
        .mp_length = (lenfunc)len,
        .mp_subscript = (binaryfunc)subscript,
        .mp_ass_subscript = (objobjargproc)subscript_assign,
    },
    .tp_init = (initproc)init,
    .tp_methods = (PyMethodDef[]){
        {"identity", (PyCFunction)identity, METH_CLASS | METH_NOARGS, NULL},
        {"transpose", (PyCFunction)transpose, METH_NOARGS, NULL},
        {"transposed", (PyCFunction)transposed, METH_NOARGS, NULL},
        {"inverse", (PyCFunction)inverse, METH_NOARGS, NULL},
        {"inversed", (PyCFunction)inversed, METH_NOARGS, NULL},
        {0},
    },
    .tp_getset = (PyGetSetDef[]){
        {"row0", (getter)row0_get, (setter)row0_set, NULL, NULL},
        {"row1", (getter)row1_get, (setter)row1_set, NULL, NULL},
        {"row2", (getter)row2_get, (setter)row2_set, NULL, NULL},
        {"row3", (getter)row3_get, (setter)row3_set, NULL, NULL},
        {0},
    },
};
