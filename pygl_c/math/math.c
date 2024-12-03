#include <math.h>
#include <cglm/util.h>
#include "quaternion.h"
#include "vector/vector.h"
#include "matrix/matrix.h"
#include "../module.h"

static PyObject* get_closest_factors(PyObject* Py_UNUSED(self), PyObject* value)
{
    if (!PyLong_Check(value))
    {
        PyErr_Format(PyExc_TypeError, "Expected value to be of type int, got: %s.", Py_TYPE(value)->tp_name);
        return NULL;
    }

    long long orig = PyLong_AsLongLong(value);
    long long val = (long long)sqrt((double)orig);
    while (orig % val != 0)
        val--;

    return PyTuple_Pack(2, PyLong_FromLongLong(val), PyLong_FromDouble(orig / (double)val));
}

static PyObject* interpolate(PyObject* Py_UNUSED(self), PyObject* args)
{
    PyObject* a = NULL;
    PyObject* b = NULL;
    float factor = 0.0f;

    if (!PyArg_ParseTuple(args, "OOf", &a, &b, &factor))
        return NULL;

    if (Py_TYPE(a) != Py_TYPE(b))
    {
        PyErr_SetString(PyExc_TypeError, "Arguments 1 and 2 have to be of the same type.");
        return NULL;
    }

    if (PyFloat_Check(a))
    {
        float res = glm_lerp(
            (float)PyFloat_AS_DOUBLE(a),
            (float)PyFloat_AS_DOUBLE(b),
            factor);
        return PyFloat_FromDouble(res);
    }
    else if (PyObject_IsInstance(a, (PyObject*)&pyVector2Type))
    {
        Vector2* res = PyObject_New(Vector2, &pyVector2Type);
        glm_vec2_lerp(
            ((Vector2*)a)->data,
            ((Vector2*)b)->data,
            factor,
            res->data);
        return (PyObject*)res;
    }
    else if (PyObject_IsInstance(a, (PyObject*)&pyVector3Type))
    {
        Vector3* res = PyObject_New(Vector3, &pyVector3Type);
        glm_vec3_lerp(
            ((Vector3*)a)->data,
            ((Vector3*)b)->data,
            factor,
            res->data);
        return (PyObject*)res;
    }
    else if (PyObject_IsInstance(a, (PyObject*)&pyVector4Type))
    {
        Vector4* res = PyObject_New(Vector4, &pyVector4Type);
        glm_vec4_lerp(
            ((Vector4*)a)->data,
            ((Vector4*)b)->data,
            factor,
            res->data);
        return (PyObject*)res;
    }
    else if (PyObject_IsInstance(a, (PyObject*)&pyQuaternionType))
    {
        Quaternion* res = PyObject_New(Quaternion, &pyQuaternionType);
        glm_quat_lerp(
            ((Quaternion*)a)->data,
            ((Quaternion*)b)->data,
            factor,
            res->data);
        return (PyObject*)res;
    }

    PyErr_Format(PyExc_TypeError, "Expected arguments to be of type Vector* or float, but got: %s", Py_TYPE(a)->tp_name);
    return NULL;
}

static PyObject* deg_to_rad(PyObject* Py_UNUSED(self), PyObject* angle)
{
    if (!PyFloat_Check(angle))
    {
        PyErr_Format(PyExc_TypeError, "Expected argument to be of type float, got %s.", Py_TYPE(angle)->tp_name);
        return NULL;
    }

    return PyFloat_FromDouble(glm_rad(PyFloat_AS_DOUBLE(angle)));
}

static PyObject* rad_to_deg(PyObject* Py_UNUSED(self), PyObject* angle)
{
    if (!PyFloat_Check(angle))
    {
        PyErr_Format(PyExc_TypeError, "Expected argument to be of type float, got %s.", Py_TYPE(angle)->tp_name);
        return NULL;
    }

    return PyFloat_FromDouble(glm_deg(PyFloat_AS_DOUBLE(angle)));
}

static ModuleInfo modInfo = {
    .def = {
        PyModuleDef_HEAD_INIT,
        .m_name = "pygl.math",
        .m_size = -1,
        .m_methods = (PyMethodDef[]) {
            {"get_closest_factors", get_closest_factors, METH_O, NULL},
            {"interpolate", interpolate, METH_VARARGS, NULL},
            {"deg_to_rad", deg_to_rad, METH_O, NULL},
            {"rad_to_deg", rad_to_deg, METH_O, NULL},
            {0}}},
    .types = (PyTypeObject*[]) {
        &pyVector2Type,
        &pyVector3Type,
        &pyVector4Type,
        &pyQuaternionType,
        &pyMatrix2Type,
        &pyMatrix3Type,
        &pyMatrix4Type,
        NULL},
};


PyMODINIT_FUNC PyInit_math()
{
    return module_create_from_info(&modInfo);
}
