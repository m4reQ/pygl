#pragma once

#ifndef MAT_LEN
#error "Matrix length not defined (MAT_LEN)"
#endif

#include <string.h>
#include <stdbool.h>
#include <cglm/cam.h>
#include <cglm/affine.h>
#include "matrix.h"
#include "../quaternion.h"
#include "../vector/vector.h"
#include "../../utility.h"

#define _TYPE CAT(Matrix, MAT_LEN)
#define _PY_TYPE CAT(CAT(pyMatrix, MAT_LEN), Type)
#define _GLM_TYPE CAT(mat, MAT_LEN)
#define _VEC_TYPE CAT(Vector, MAT_LEN)
#define _VEC_PY_TYPE CAT(CAT(pyVector, MAT_LEN), Type)
#define _SELF _TYPE *self
#define _GLM_INVOKE(func, ...) CAT(CAT(CAT(glm_mat, MAT_LEN), _), func)(__VA_ARGS__)
#define _NEW(var)                                \
    _TYPE *var = PyObject_New(_TYPE, &_PY_TYPE); \
    var->length = MAT_LEN * MAT_LEN
#define _GLM_VEC_COPY(src, dst) CAT(CAT(glm_vec, MAT_LEN), _copy)(src, dst)
#define _MAT_OP_CHECK_TYPE(obj)                                           \
    do                                                                    \
    {                                                                     \
        if (!PyObject_IsInstance((PyObject *)obj, (PyObject *)&_PY_TYPE)) \
            Py_RETURN_NOTIMPLEMENTED;                                     \
    } while (0)

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
            if (PyObject_GetBuffer(arg, &buf, PyBUF_C_CONTIGUOUS) == -1)
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

    self->length = MAT_LEN * MAT_LEN;

    return 0;
}

static PyObject *identity(PyTypeObject *cls, PyObject *Py_UNUSED(args))
{
    _TYPE *res = PyObject_New(_TYPE, cls);
    _GLM_INVOKE(identity, res->data);

    res->length = MAT_LEN * MAT_LEN;

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
    _MAT_OP_CHECK_TYPE(other);

    _GLM_TYPE res;
    _GLM_INVOKE(mul, self->data, ((_TYPE *)other)->data, res);
    _GLM_INVOKE(copy, res, self->data);

    return (PyObject *)self;
}

static PyObject *add(_SELF, Matrix4 *other)
{
    _MAT_OP_CHECK_TYPE(other);

    float *data = &self->data[0];
    float *otherData = &other->data[0];

    _NEW(res);
    for (size_t i = 0; i < MAT_LEN * MAT_LEN; i++)
        ((float *)res->data)[i] = data[i] + otherData[i];

    return (PyObject *)res;
}

static PyObject *iadd(_SELF, Matrix4 *other)
{
    _MAT_OP_CHECK_TYPE(other);

    float *data = &self->data[0];
    float *otherData = &other->data[0];

    for (size_t i = 0; i < MAT_LEN * MAT_LEN; i++)
        data[i] = data[i] + otherData[i];

    return (PyObject *)self;
}

static PyObject *sub(_SELF, Matrix4 *other)
{
    _MAT_OP_CHECK_TYPE(other);

    float *data = &self->data[0];
    float *otherData = &other->data[0];

    _NEW(res);
    for (size_t i = 0; i < MAT_LEN * MAT_LEN; i++)
        ((float *)res->data)[i] = data[i] - otherData[i];

    return (PyObject *)res;
}

static PyObject *isub(_SELF, Matrix4 *other)
{
    _MAT_OP_CHECK_TYPE(other);

    float *data = &self->data[0];
    float *otherData = &other->data[0];

    for (size_t i = 0; i < MAT_LEN * MAT_LEN; i++)
        data[i] = data[i] - otherData[i];

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
    return self->length;
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

static int get_buffer(_SELF, Py_buffer *buffer, int flags)
{
    *buffer = (Py_buffer){
        .obj = Py_NewRef(self),
        .buf = self->data,
        .len = MAT_LEN * MAT_LEN * sizeof(float),
        .itemsize = sizeof(float),
        .ndim = 2,
        .readonly = !(flags & PyBUF_WRITABLE == PyBUF_WRITABLE),
        .format = (flags & PyBUF_FORMAT == PyBUF_FORMAT) ? "f" : NULL,
        .shape = (Py_ssize_t[]){MAT_LEN, MAT_LEN},
        .strides = (Py_ssize_t[]){sizeof(float), 4 * sizeof(float)},
    };

    return 0;
}

static Matrix4 *ortho(PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {"left", "right", "bottom", "top", "z_near", "z_far", NULL};

    float left, right, bottom, top;
    float zNear = -1.0f;
    float zFar = 1.0f;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "ffff|ff", kwNames, &left, &right, &bottom, &top, &zNear, &zFar))
        return NULL;

    _NEW(result);

    glm_ortho(
        left,
        right,
        bottom,
        top,
        zNear,
        zFar,
        result->data);

    return result;
}

static Matrix4 *perspective(PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {"fov", "aspect", "z_near", "z_far", NULL};

    float fov, aspect;
    float zNear = -1.0f;
    float zFar = 1.0f;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "ff|ff", kwNames, &fov, &aspect, &zNear, &zFar))
        return NULL;

    _NEW(result);

    glm_perspective(
        fov,
        aspect,
        zNear,
        zFar,
        result->data);

    return result;
}

static Matrix4 *look_at(PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {"eye", "center", "up", NULL};

    Vector3 *eye, *center;
    Vector3 *up = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!|O!", kwNames, &pyVector3Type, &eye, &pyVector3Type, &center, &pyVector3Type, &up))
        return NULL;

    vec3 *upData = (up != NULL) ? &up->data : &(vec3){0.0f, 1.0f, 0.0f};

    _NEW(result);
    glm_lookat(eye->data, center->data, *upData, result->data);

    return result;
}

static Matrix4 *look(PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    Matrix4 *result;

    if (PyTuple_GET_SIZE(args) == 2) // Matrix4.look(Vector3, Quaternion)
    {
        Vector3 *eye;
        Quaternion *orientation;
        if (!PyArg_ParseTuple(args, "O!O!", &pyVector3Type, &eye, &pyQuaternionType, &orientation))
            return NULL;

        _NEW(result);
        glm_quat_look(eye->data, orientation->data, result->data);
    }
    else // Matrix4.look(Vector3, Vector3, Vector3 = Vector3(0.0, 1.0, 0.0))
    {
        static char *kwNames[] = {"eye", "dir", "up", NULL};

        Vector3 *eye, *dir;
        Vector3 *up = NULL;
        if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!|O!", kwNames, &pyVector3Type, &eye, &pyVector3Type, &dir, &pyVector3Type, &up))
            return NULL;

        vec3 *upData = (up != NULL) ? &up->data : &(vec3){0.0f, 1.0f, 0.0f};

        _NEW(result);
        glm_look(eye->data, dir->data, *upData, result->data);
    }

    return result;
}

static Matrix4 *transform(PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    size_t argsCount = PyTuple_GET_SIZE(args);
    if (argsCount == 0)
    {
        PyErr_SetString(PyExc_TypeError, "Function requires at least one argument.");
        return NULL;
    }

    mat4 matrix = GLM_MAT4_IDENTITY_INIT;
    bool hasScale = false;

    if (argsCount >= 1)
    {
        Vector3 *translation = PyTuple_GET_ITEM(args, 0);
        if (!PyObject_IsInstance(translation, &pyVector3Type))
        {
            PyErr_Format(PyExc_TypeError, "Expected argument 1 to be of type pygl.math.Vector3, got: %s.", Py_TYPE(translation)->tp_name);
            return NULL;
        }

        glm_translate(matrix, translation->data);
    }
    if (argsCount >= 2)
        hasScale = true; // we have to defer applying scale to keep valid transformation order
    if (argsCount >= 3)
    {
        Quaternion *rotation = PyTuple_GET_ITEM(args, 2);
        if (!PyObject_IsInstance(rotation, &pyQuaternionType))
        {
            PyErr_Format(PyExc_TypeError, "Expected argument 3 to be of type pygl.math.Quaternion, got: %s.", Py_TYPE(rotation)->tp_name);
            return NULL;
        }

        glm_quat_rotate(matrix, rotation->data, matrix);
    }

    if (hasScale)
    {
        Vector3 *scale = PyTuple_GET_ITEM(args, 1);
        if (!PyObject_IsInstance(scale, &pyVector3Type))
        {
            PyErr_Format(PyExc_TypeError, "Expected argument 1 to be of type pygl.math.Vector3, got: %s.", Py_TYPE(scale)->tp_name);
            return NULL;
        }

        glm_scale(matrix, scale->data);
    }

    _NEW(result);
    glm_mat4_copy(matrix, result->data);

    return result;
}

static PyObject *class_length(PyTypeObject *cls, PyObject *Py_UNUSED(args))
{
    return PyLong_FromLong(MAT_LEN * MAT_LEN);
}

PyTypeObject _PY_TYPE = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_new = PyType_GenericNew,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_basicsize = sizeof(_TYPE),
    .tp_name = "pygl.math.Matrix" STRINGIFY(MAT_LEN),
    .tp_init = (initproc)init,
    .tp_as_buffer = &(PyBufferProcs){
        .bf_getbuffer = (getbufferproc)get_buffer,
    },
    .tp_as_number = &(PyNumberMethods){
        .nb_matrix_multiply = (binaryfunc)matmul,
        .nb_inplace_matrix_multiply = (binaryfunc)imatmul,
        .nb_add = (binaryfunc)add,
        .nb_inplace_add = (binaryfunc)iadd,
        .nb_subtract = (binaryfunc)sub,
        .nb_inplace_subtract = (binaryfunc)isub,
    },
    .tp_as_mapping = &(PyMappingMethods){
        .mp_length = (lenfunc)len,
        .mp_subscript = (binaryfunc)subscript,
        .mp_ass_subscript = (objobjargproc)subscript_assign,
    },
    .tp_methods = (PyMethodDef[]){
        {"identity", (PyCFunction)identity, METH_CLASS | METH_NOARGS, NULL},
        {"transpose", (PyCFunction)transpose, METH_NOARGS, NULL},
        {"transposed", (PyCFunction)transposed, METH_NOARGS, NULL},
        {"inverse", (PyCFunction)inverse, METH_NOARGS, NULL},
        {"inversed", (PyCFunction)inversed, METH_NOARGS, NULL},
        {"length", (PyCFunction)class_length, METH_CLASS | METH_NOARGS, NULL},
#if MAT_LEN == 4
        {"transform", (PyCFunction)transform, METH_VARARGS | METH_CLASS, NULL},
        {"look", (PyCFunction)look, METH_VARARGS | METH_CLASS, NULL},
        {"look_at", (PyCFunction)look_at, METH_VARARGS | METH_CLASS, NULL},
        {"ortho", (PyCFunction)ortho, METH_VARARGS | METH_CLASS | METH_KEYWORDS, NULL},
        {"perspective", (PyCFunction)perspective, METH_VARARGS | METH_CLASS | METH_KEYWORDS, NULL},
#endif
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
