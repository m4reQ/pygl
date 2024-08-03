#include "quaternion.h"
#include "vector/vector.h"

#define NEW_QUAT(var) Quaternion *var = PyObject_New(Quaternion, &pyQuaternionType)

void py_quaternion_copy(void *dst, const Quaternion *quaternion)
{
    glm_quat_copy(quaternion, dst);
}

static int init(Quaternion *self, PyObject *args, PyObject *Py_UNUSED(kwargs))
{
    if (!PyArg_ParseTuple(args, "ffff", &self->data[0], &self->data[1], &self->data[2], &self->data[3]))
        return -1;

    return 0;
}

static PyObject *identity(PyTypeObject *cls, PyObject *Py_UNUSED(args))
{
    Quaternion *res = PyObject_New(Quaternion, cls);
    glm_quat_identity(res->data);

    return (PyObject *)res;
}

static PyObject *from_axis(PyTypeObject *cls, PyObject *args)
{
    float angle;
    Vector3 *axis;
    if (!PyArg_ParseTuple(args, "fO!", &angle, &pyVector3Type, &axis))
        return NULL;

    Quaternion *res = PyObject_New(Quaternion, cls);
    glm_quatv(res->data, angle, axis->data);

    return (PyObject *)res;
}

static PyObject *normalize(Quaternion *self, PyObject *Py_UNUSED(args))
{
    glm_quat_normalize(self->data);
    Py_RETURN_NONE;
}

static PyObject *normalized(Quaternion *self, PyObject *Py_UNUSED(args))
{
    NEW_QUAT(res);
    glm_quat_normalize_to(self->data, res->data);

    return (PyObject *)res;
}

static PyObject *dot(Quaternion *self, PyObject *other)
{
    if (!PyObject_IsInstance(other, (PyObject *)&pyQuaternionType))
    {
        PyErr_Format(PyExc_TypeError, "Expected argument to be of type pygl.math.Quaternion, but got %s.", Py_TYPE(other)->tp_name);
        return NULL;
    }

    float res = glm_quat_dot(self->data, ((Quaternion *)other)->data);
    return PyFloat_FromDouble(res);
}

static PyObject *conjugate(Quaternion *self, PyObject *Py_UNUSED(args))
{
    versor res;
    glm_quat_conjugate(self->data, res);
    glm_quat_copy(res, self->data);

    Py_RETURN_NONE;
}

static PyObject *inverse(Quaternion *self, PyObject *Py_UNUSED(args))
{
    versor res;
    glm_quat_inv(self->data, res);
    glm_quat_copy(res, self->data);

    Py_RETURN_NONE;
}

static PyObject *inversed(Quaternion *self, PyObject *Py_UNUSED(args))
{
    NEW_QUAT(res);
    glm_quat_inv(self->data, res->data);

    return (PyObject *)res;
}

static PyObject *look_at(PyTypeObject *cls, PyObject *args)
{
    Vector3 *dir;
    Vector3 *up;
    if (!PyArg_ParseTuple(args, "O!O!", &pyVector3Type, &dir, &pyVector3Type, &up))
        return NULL;

    Quaternion *res = PyObject_New(Quaternion, cls);
    glm_quat_for(dir->data, up->data, res->data);

    return (PyObject *)res;
}

static PyObject *interpolate(Quaternion *self, PyObject *args)
{
    Quaternion *other;
    float factor;
    if (!PyArg_ParseTuple(args, "O!f", &pyQuaternionType, &other, &factor))
        return NULL;

    NEW_QUAT(res);
    glm_quat_lerp(self->data, other->data, factor, res->data);

    return (PyObject *)res;
}

#pragma region GETTERS
static PyObject *angle_get(Quaternion *self, void *Py_UNUSED(closure))
{
    return PyFloat_FromDouble(glm_quat_angle(self->data));
}

static PyObject *axis_get(Quaternion *self, void *Py_UNUSED(closure))
{
    Vector3 *result = PyObject_New(Vector3, &pyVector3Type);
    glm_quat_axis(self->data, result->data);

    return (PyObject *)result;
}

static PyObject *real_get(Quaternion *self, void *Py_UNUSED(closure))
{
    return PyFloat_FromDouble(glm_quat_real(self->data));
}

static PyObject *imag_get(Quaternion *self, void *Py_UNUSED(closure))
{
    Vector3 *result = PyObject_New(Vector3, &pyVector3Type);
    glm_quat_imag(self->data, result->data);

    return (PyObject *)result;
}
#pragma endregion

#pragma region NB_METHODS
static PyObject *mul(Quaternion *self, PyObject *other)
{
    if (!PyObject_IsInstance(other, (PyObject *)&pyQuaternionType))
        Py_RETURN_NOTIMPLEMENTED;

    NEW_QUAT(res);
    glm_quat_mul(self->data, ((Quaternion *)other)->data, res->data);

    return (PyObject *)res;
}

static PyObject *imul(Quaternion *self, PyObject *other)
{
    if (!PyObject_IsInstance(other, (PyObject *)&pyQuaternionType))
        Py_RETURN_NOTIMPLEMENTED;

    versor res;
    glm_quat_mul(self->data, ((Quaternion *)other)->data, res);
    glm_quat_copy(res, self->data);

    return (PyObject *)self;
}

static PyObject *sub(Quaternion *self, PyObject *other)
{
    if (!PyObject_IsInstance(other, (PyObject *)&pyQuaternionType))
        Py_RETURN_NOTIMPLEMENTED;

    NEW_QUAT(res);
    glm_quat_sub(self->data, ((Quaternion *)other)->data, res->data);

    return (PyObject *)res;
}

static PyObject *isub(Quaternion *self, PyObject *other)
{
    if (!PyObject_IsInstance(other, (PyObject *)&pyQuaternionType))
        Py_RETURN_NOTIMPLEMENTED;

    versor res;
    glm_quat_sub(self->data, ((Quaternion *)other)->data, res);
    glm_quat_copy(res, self->data);

    return (PyObject *)self;
}

static PyObject *add(Quaternion *self, PyObject *other)
{
    if (!PyObject_IsInstance(other, (PyObject *)&pyQuaternionType))
        Py_RETURN_NOTIMPLEMENTED;

    NEW_QUAT(res);
    glm_quat_add(self->data, ((Quaternion *)other)->data, res->data);

    return (PyObject *)res;
}

static PyObject *iadd(Quaternion *self, PyObject *other)
{
    if (!PyObject_IsInstance(other, (PyObject *)&pyQuaternionType))
        Py_RETURN_NOTIMPLEMENTED;

    versor res;
    glm_quat_add(self->data, ((Quaternion *)other)->data, res);
    glm_quat_copy(res, self->data);

    return (PyObject *)self;
}
#pragma endregion

static PyObject *repr(Quaternion *self)
{
    // basic string len + ptr len + 4 * float len
    char buf[58 + 14 + 4 * 7];
    sprintf(
        buf,
        "<object pygl.math.Quaternion (x: %.5f, y: %.5f, z: %.5f, w: %.5fj) at 0x%p>",
        self->data[0],
        self->data[1],
        self->data[2],
        self->data[3],
        self);
    return PyUnicode_FromString(buf);
}

PyTypeObject pyQuaternionType = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_new = PyType_GenericNew,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_basicsize = sizeof(Quaternion),
    .tp_name = "pygl.math.Quaternion",
    .tp_init = (initproc)init,
    .tp_repr = (reprfunc)repr,
    .tp_members = (PyMemberDef[]){
        {"x", T_FLOAT, offsetof(Quaternion, data[0]), 0, NULL},
        {"y", T_FLOAT, offsetof(Quaternion, data[1]), 0, NULL},
        {"z", T_FLOAT, offsetof(Quaternion, data[2]), 0, NULL},
        {"w", T_FLOAT, offsetof(Quaternion, data[3]), 0, NULL},
        {0},
    },
    .tp_methods = (PyMethodDef[]){
        {"from_axis", (PyCFunction)from_axis, METH_CLASS | METH_VARARGS, NULL},
        {"identity", (PyCFunction)identity, METH_CLASS | METH_NOARGS, NULL},
        {"look_at", (PyCFunction)look_at, METH_CLASS | METH_VARARGS, NULL},
        {"normalize", (PyCFunction)normalize, METH_NOARGS, NULL},
        {"normalized", (PyCFunction)normalized, METH_NOARGS, NULL},
        {"dot", (PyCFunction)dot, METH_O, NULL},
        {"conjugate", (PyCFunction)conjugate, METH_NOARGS, NULL},
        {"inverse", (PyCFunction)inverse, METH_NOARGS, NULL},
        {"inversed", (PyCFunction)inversed, METH_NOARGS, NULL},
        {"interpolate", (PyCFunction)interpolate, METH_VARARGS, NULL},
        {0},
    },
    .tp_getset = (PyGetSetDef[]){
        {"angle", (getter)angle_get, NULL, NULL, NULL},
        {"axis", (getter)axis_get, NULL, NULL, NULL},
        {"real", (getter)real_get, NULL, NULL, NULL},
        {"imag", (getter)imag_get, NULL, NULL, NULL},
        {0},
    },
    .tp_as_number = &(PyNumberMethods){
        .nb_multiply = (binaryfunc)mul,
        .nb_inplace_multiply = (binaryfunc)imul,
        .nb_subtract = (binaryfunc)sub,
        .nb_inplace_subtract = (binaryfunc)isub,
        .nb_add = (binaryfunc)add,
        .nb_inplace_add = (binaryfunc)iadd,
    },
};
