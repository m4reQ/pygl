#include "bufferBase.h"
#include "../utility.h"

static PyObject* py_buffer_delete(PyBufferBase* self, PyObject* Py_UNUSED(args))
{
    PyMem_Free(self->dataPtr);
    delete_gl_buffer(self->id);

    self->dataPtr = NULL;
    self->id = 0;

    Py_RETURN_NONE;
}

static PyObject* py_buffer_repr(PyBufferBase* self)
{
    return PyUnicode_FromFormat(
        "<%s [size: %d, current offset: %d] at %X>",
        Py_TYPE(self)->tp_name,
        self->size,
        self->currentOffset,
        (uintptr_t)self);
}

static PyObject* py_buffer_store(PyBufferBase* self, PyObject* data)
{
    return store_to_buffer(self, data) ? Py_NewRef(Py_None) : NULL;
}

static PyObject* py_buffer_transfer(PyBufferBase* self, PyObject* Py_UNUSED(args))
{
    glNamedBufferSubData(self->id, 0, self->currentOffset, self->dataPtr);
    self->currentOffset = 0;

    Py_RETURN_NONE;
}

static int py_buffer_init(PyBufferBase* self, PyObject* args, PyObject* Py_UNUSED(kwargs))
{
    if (!PyArg_ParseTuple(args, "K", &self->size))
        return -1;

    self->flags = GL_DYNAMIC_STORAGE_BIT;
    self->id = create_gl_buffer(self->size, self->flags);
    ThrowIf(
        self->id == 0,
        PyExc_RuntimeError,
        "Couldn't create GL buffer id.",
        -1);

    self->dataPtr = PyMem_Calloc(self->size, sizeof(char));
    // NOTE: handle out of memory here?

    return 0;
}

static void py_buffer_dealloc(PyBufferBase* self)
{
    py_buffer_delete(self, NULL);
    Py_TYPE(self)->tp_free(self);
}

static PyObject* py_buffer_reset_offset(PyBufferBase* self, PyObject* Py_UNUSED(args))
{
    self->currentOffset = 0;
    Py_RETURN_NONE;
}

static PyMethodDef pyBufferMethods[] = {
    {"delete", py_buffer_delete, METH_NOARGS, NULL},
    {"store", py_buffer_store, METH_O, NULL},
    {"transfer", py_buffer_transfer, METH_NOARGS, NULL},
    {"reset_offset", py_buffer_reset_offset, METH_NOARGS, NULL},
    // {"read", py_mappedbuffer_read, METH_VARARGS, NULL},
    {0},
};

PyTypeObject pyBufferType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_new = PyType_GenericNew,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_name = "pygl.buffers.Buffer",
    .tp_basicsize = sizeof(PyBufferBase),
    .tp_init = py_buffer_init,
    .tp_dealloc = py_buffer_dealloc,
    .tp_repr = py_buffer_repr,
    .tp_members = pyBufferBaseMembers,
    .tp_methods = pyBufferMethods,
};
