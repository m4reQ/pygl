#include "textureBuffer.h"
#include "../utility.h"

static PyObject *bind(PyTextureBuffer *self, PyObject *textureUnit)
{
    THROW_IF(
        !PyLong_Check(textureUnit),
        PyExc_TypeError,
        "texture_unit has to be of type int.",
        NULL);

    glBindTextureUnit(PyLong_AsUnsignedLong(textureUnit), self->textureId);

    Py_RETURN_NONE;
}

static PyMethodDef methods[] = {
    {"bind", (PyCFunction)bind, METH_O, NULL},
    {0},
};

PyTypeObject pyTextureBufferType = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_flags = Py_TPFLAGS_BASETYPE,
    .tp_new = PyType_GenericNew,
    .tp_basicsize = sizeof(PyTextureBuffer),
    .tp_name = "pygl.buffers.TextureBuffer",
    .tp_methods = methods,
};
