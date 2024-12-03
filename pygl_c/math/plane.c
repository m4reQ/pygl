#include "plane.h"
#include "vector/vector.h"

static int init(Plane *self, PyObject *args, PyObject *kwargs)
{
    // Plane(Vector3, float)
    Vector3 *normal = NULL;
    if (!PyArg_ParseTuple(args, "O!f", &pyVector3Type, &normal, &self->distance))
        return -1;

    glm_vec3_copy(normal->data, self->normal);

    return 0;
}

static PyObject *repr(Plane *self)
{
    const size_t length = strlen(Py_TYPE(self)->tp_name) + 8 * 4 + 15;
    char *buffer = PyMem_Malloc(sizeof(char) * length);
    snprintf(
        buffer,
        length,
        "%s (n: %.6g, %.6g, %.6g; d: %.6g)",
        Py_TYPE(self)->tp_name,
        self->normal[0],
        self->normal[1],
        self->normal[2],
        self->distance);

    PyObject *result = PyUnicode_FromStringAndSize(buffer, length);
    PyMem_Free(buffer);

    return result;
}

static Vector3 *get_normal(Plane *self, void *Py_UNUSED(closure))
{
    Vector3 *pos = PyObject_New(Vector3, &pyVector3Type);
    pos->length = 3;

    glm_vec3_copy(self->normal, pos->data);

    return pos;
}

static int set_normal(Plane *self, Vector3 *value, void *closure)
{
    if (!PyObject_IsInstance((PyObject *)value, (PyObject *)&pyVector3Type))
    {
        PyErr_Format(PyExc_TypeError, "Expected value to be of type pygl.math.Vector3, got: %s.", Py_TYPE(value)->tp_name);
        return -1;
    }

    glm_vec3_copy(value->data, self->normal);

    return 0;
}

PyTypeObject pyPlaneType = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_name = "pygl.math.Plane",
    .tp_itemsize = sizeof(Plane),
    .tp_init = (initproc)init,
    .tp_repr = (reprfunc)repr,
    .tp_getset = (PyGetSetDef[]){
        {"normal", (getter)get_normal, (setter)set_normal, NULL, NULL},
        {0},
    },
    .tp_methods = (PyMethodDef[]){
        {0},
    },
    .tp_members = (PyMemberDef[]){
        {"normal_x", Py_T_FLOAT, offsetof(Plane, normal[0]), 0, NULL},
        {"normal_y", Py_T_FLOAT, offsetof(Plane, normal[1]), 0, NULL},
        {"normal_z", Py_T_FLOAT, offsetof(Plane, normal[2]), 0, NULL},
        {"distance", Py_T_FLOAT, offsetof(Plane, distance), 0, NULL},
        {0},
    },
};
