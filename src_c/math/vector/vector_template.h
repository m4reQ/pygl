#pragma once

#ifndef VEC_LEN
#error "Vector length not defined (VEC_LEN)"
#endif

#include "vector.h"
#include "../../utility.h"

#define _TYPE CAT(Vector, VEC_LEN)
#define _PY_TYPE CAT(CAT(pyVector, VEC_LEN), Type)
#define _GLM_TYPE CAT(vec, VEC_LEN)
#define _SELF _TYPE *self
#define _NEW(var) _TYPE *var = PyObject_New(_TYPE, &_PY_TYPE)
#define _GLM_INVOKE(func, ...) CAT(CAT(CAT(glm_vec, VEC_LEN), _), func)(__VA_ARGS__)
#define _VEC_CHECK(o)                                                                                                                              \
    do                                                                                                                                             \
    {                                                                                                                                              \
        if (!PyObject_IsInstance((PyObject *)o, (PyObject *)&_PY_TYPE))                                                                            \
        {                                                                                                                                          \
            PyErr_Format(PyExc_TypeError, "Excepted argument to be of type pygl.math.Vector" STRINGIFY(VEC_LEN) " got: %s.", Py_TYPE(o)->tp_name); \
            return NULL;                                                                                                                           \
        }                                                                                                                                          \
    } while (0)
#define _VEC_DATA_OFFSET(s, n) offsetof(s, data) + sizeof(float) * n
#define _VEC_CHECK_IDX(idx, len, ret)                                 \
    do                                                                \
    {                                                                 \
        if (idx > len - 1)                                            \
        {                                                             \
            PyErr_SetString(PyExc_IndexError, "Index out of range."); \
            return ret;                                               \
        }                                                             \
    } while (0)

static int vec_init(_SELF, PyObject *args, PyObject *_kwargs)
{
    if (!PyArg_ParseTuple(args, STRINGIFY(INIT_ARGS), INIT_OUT))
        return -1;

    return 1;
}

static PyObject *vec_normalize(_SELF, PyObject *_args)
{
    _GLM_INVOKE(normalize, self->data);
    Py_RETURN_NONE;
}

static PyObject *vec_normalized(_SELF, PyObject *_args)
{
    _GLM_TYPE n;
    _GLM_INVOKE(normalize_to, self->data, n);

    _TYPE *result = PyObject_New(_TYPE, &_PY_TYPE);
    memcpy(result->data, n, sizeof(_GLM_TYPE));

    return (PyObject *)result;
}

static PyObject *vec_dot(_SELF, _TYPE *other)
{
    _VEC_CHECK(other);
    float result = _GLM_INVOKE(dot, self->data, other->data);
    return PyFloat_FromDouble((double)result);
}

#if VEC_LEN != 4
static PyObject *vec_cross(_SELF, _TYPE *other)
{
    _VEC_CHECK(other);

#if VEC_LEN == 2
    float result = _GLM_INVOKE(cross, self->data, other->data);
    return PyFloat_FromDouble((double)result);
#else
    _GLM_TYPE
    n;
    _GLM_INVOKE(cross, self->data, other->data, n);

    _TYPE *result = PyObject_New(_TYPE, &_PY_TYPE);
    memcpy(result->data, n, sizeof(_GLM_TYPE));

    return (PyObject *)result;
#endif
}
#endif

static PyObject *vec_distance(_SELF, _TYPE *other)
{
    _VEC_CHECK(other);
    float result = _GLM_INVOKE(distance, self->data, other->data);
    return PyFloat_FromDouble((double)result);
}

static PyObject *vec_interpolate(_SELF, PyObject *args)
{
    _TYPE *other = NULL;
    float factor = 0.0f;
    if (!PyArg_ParseTuple(args, "O!f", &_PY_TYPE, &other, &factor))
        return NULL;

    _GLM_TYPE n;
    _GLM_INVOKE(lerp, self->data, other->data, factor, n);

    _TYPE *result = PyObject_New(_TYPE, &_PY_TYPE);
    memcpy(result->data, n, sizeof(_GLM_TYPE));

    return (PyObject *)result;
}

static PyObject *vec_one(PyTypeObject *cls, PyObject *_args)
{
    _TYPE *result = PyObject_New(_TYPE, cls);
    for (size_t i = 0; i < VEC_LEN; i++)
        result->data[i] = 1.0f;

    return (PyObject *)result;
}

static PyObject *vec_zero(PyTypeObject *cls, PyObject *_args)
{
    _TYPE *result = PyObject_New(_TYPE, cls);
    memset(result->data, 0, sizeof(_GLM_TYPE));

    return (PyObject *)result;
}

static Py_ssize_t vec_len(_SELF)
{
    return VEC_LEN;
}

static PyObject *vec_item(_SELF, Py_ssize_t index)
{
    _VEC_CHECK_IDX(index, VEC_LEN, NULL);
    return PyFloat_FromDouble((double)self->data[index]);
}

static int vec_assign_item(_SELF, Py_ssize_t index, PyObject *value)
{
    _VEC_CHECK_IDX(index, VEC_LEN, -1);
    if (!PyFloat_Check(value))
    {
        PyErr_Format(PyExc_TypeError, "Excepted value to be of type float, got: %s.", Py_TYPE(value)->tp_name);
        return -1;
    }

    self->data[index] = (float)PyFloat_AS_DOUBLE(value);
    return 0;
}

static PyObject *vec_add(_SELF, PyObject *other)
{
    if (!PyObject_IsInstance(other, (PyObject *)&_PY_TYPE))
        Py_RETURN_NOTIMPLEMENTED;

    _TYPE *res = PyObject_New(_TYPE, &_PY_TYPE);
    _GLM_INVOKE(add, self->data, ((_TYPE *)other)->data, res->data);

    return (PyObject *)res;
}

static PyObject *vec_sub(_SELF, PyObject *other)
{
    if (!PyObject_IsInstance(other, (PyObject *)&_PY_TYPE))
        Py_RETURN_NOTIMPLEMENTED;

    _TYPE *res = PyObject_New(_TYPE, &_PY_TYPE);
    _GLM_INVOKE(sub, self->data, ((_TYPE *)other)->data, res->data);

    return (PyObject *)res;
}

static PyObject *vec_iadd(_SELF, PyObject *other)
{
    if (!PyObject_IsInstance(other, (PyObject *)&_PY_TYPE))
        Py_RETURN_NOTIMPLEMENTED;

    _GLM_INVOKE(add, self->data, ((_TYPE *)other)->data, self->data);

    return (PyObject *)self;
}

static PyObject *vec_isub(_SELF, PyObject *other)
{
    if (!PyObject_IsInstance(other, (PyObject *)&_PY_TYPE))
        Py_RETURN_NOTIMPLEMENTED;

    _GLM_INVOKE(sub, self->data, ((_TYPE *)other)->data, self->data);

    return (PyObject *)self;
}

static PyObject *vec_repr(_SELF)
{
    char floatFormatBuffer[64] = {0};
    snprintf(floatFormatBuffer, _countof(floatFormatBuffer), FORMAT_STRING, FORMAT_DATA);

    return PyUnicode_FromFormat(
        "<%s object at %p (%s)>",
        Py_TYPE(self)->tp_name,
        self,
        floatFormatBuffer);
}

static PyObject *get_data_at(_SELF, size_t index)
{
    return PyFloat_FromDouble(self->data[index]);
}

static bool set_data_at(_SELF, size_t index, PyObject *value)
{
    if (!PyFloat_Check(value))
    {
        PyErr_Format(PyExc_TypeError, "Expected value to be of type float, got: %s.", Py_TYPE(value)->tp_name);
        return false;
    }

    self->data[index] = (float)PyFloat_AS_DOUBLE(value);

    return true;
}

static PyObject *r_get(_SELF, void *Py_UNUSED(closure))
{
    return get_data_at(self, 0);
}

static PyObject *r_set(_SELF, PyObject *value, void *Py_UNUSED(closure))
{
    return set_data_at(self, 0, value) ? 0 : -1;
}

static PyObject *g_get(_SELF, void *Py_UNUSED(closure))
{
    return get_data_at(self, 1);
}

static PyObject *g_set(_SELF, PyObject *value, void *Py_UNUSED(closure))
{
    return set_data_at(self, 1, value) ? 0 : -1;
}

static PyObject *b_get(_SELF, void *Py_UNUSED(closure))
{
    return get_data_at(self, 2);
}

static PyObject *b_set(_SELF, PyObject *value, void *Py_UNUSED(closure))
{
    return set_data_at(self, 2, value) ? 0 : -1;
}

static PyObject *a_get(_SELF, void *Py_UNUSED(closure))
{
    return get_data_at(self, 3);
}

static PyObject *a_set(_SELF, PyObject *value, void *Py_UNUSED(closure))
{
    return set_data_at(self, 3, value) ? 0 : -1;
}

PyTypeObject _PY_TYPE = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_new = PyType_GenericNew,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_name = "pygl.math." STRINGIFY(_TYPE),
    .tp_basicsize = sizeof(_TYPE),
    .tp_init = (initproc)vec_init,
    .tp_repr = (reprfunc)vec_repr,
    .tp_getset = (PyGetSetDef[]){
        {"r", (getter)r_get, (setter)r_set, NULL, NULL},
        {"g", (getter)g_get, (setter)g_set, NULL, NULL},
#if VEC_LEN > 2
        {"b", (getter)b_get, (setter)b_set, NULL, NULL},
#endif
#if VEC_LEN > 3
        {"a", (getter)a_get, (setter)a_set, NULL, NULL},
#endif
        {0},
    },
    .tp_members = (PyMemberDef[]){
        {"x", T_FLOAT, _VEC_DATA_OFFSET(_TYPE, 0), 0, NULL},
        {"y", T_FLOAT, _VEC_DATA_OFFSET(_TYPE, 1), 0, NULL},
#if VEC_LEN > 2
        {"z", T_FLOAT, _VEC_DATA_OFFSET(_TYPE, 2), 0, NULL},
#endif
#if VEC_LEN > 3
        {"w", T_FLOAT, _VEC_DATA_OFFSET(_TYPE, 3), 0, NULL},
#endif
        {0},
    },
    .tp_methods = (PyMethodDef[]){
        {"normalize", (PyCFunction)vec_normalize, METH_NOARGS, NULL},
        {"normalized", (PyCFunction)vec_normalized, METH_NOARGS, NULL},
        {"dot", (PyCFunction)vec_dot, METH_O, NULL},
#if VEC_LEN != 4
        {"cross", (PyCFunction)vec_cross, METH_O, NULL},
#endif
        {"distance", (PyCFunction)vec_distance, METH_O, NULL},
        {"interpolate", (PyCFunction)vec_interpolate, METH_VARARGS, NULL},
        {"zero", (PyCFunction)vec_zero, METH_CLASS | METH_NOARGS, NULL},
        {"one", (PyCFunction)vec_one, METH_CLASS | METH_NOARGS, NULL},
        {0},
    },
    .tp_as_sequence = &(PySequenceMethods){
        .sq_length = (lenfunc)VEC_LEN,
        .sq_item = (ssizeargfunc)vec_item,
        .sq_ass_item = (ssizeobjargproc)vec_assign_item,
    },
    .tp_as_number = &(PyNumberMethods){
        .nb_add = (binaryfunc)vec_add,
        .nb_subtract = (binaryfunc)vec_sub,
        .nb_inplace_add = (binaryfunc)vec_iadd,
        .nb_inplace_subtract = (binaryfunc)vec_isub,
    },
};
