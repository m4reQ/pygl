#include "mappedBuffer.h"
#include "../utility.h"

static int py_mappedbuffer_init(PyBufferBase* self, PyObject* args, PyObject* kwargs)
{
    static char* kwNames[] = {"size", "readable", "writable", "persistent", NULL};

    bool isReadable;
    bool isWritable;
    bool isPersistent = false;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Kpp|p", kwNames, &self->size, &isReadable, &isWritable, &isPersistent))
        return -1;

    if (isReadable)
        self->flags |= GL_MAP_READ_BIT;
    if (isWritable)
        self->flags |= GL_MAP_WRITE_BIT;
    if (isPersistent)
        self->flags |= GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    self->id = create_gl_buffer(self->size, self->flags);
    ThrowIf(
        self->id == 0,
        PyExc_RuntimeError,
        "Couldn't create GL buffer id.",
        -1);

    if (isPersistent)
    {
        self->dataPtr = map_gl_buffer(self->id, 0, self->size, self->flags);
        ThrowIf(
            self->dataPtr == NULL,
            PyExc_RuntimeError,
            "Couldn't persistently map buffer.",
            -1);
    }

    return 0;
}

static PyObject* py_mappedbuffer_delete(PyBufferBase* self, PyObject* Py_UNUSED(self))
{
    delete_gl_buffer(self->id);

    self->dataPtr = NULL;
    self->id = 0;

    Py_RETURN_NONE;
}

static void py_mappedbuffer_dealloc(PyBufferBase* self)
{
    py_mappedbuffer_delete(self, NULL);
    Py_TYPE(self)->tp_free(self);
}

static PyObject* py_mappedbuffer_repr(PyBufferBase* self)
{
    const char* state;
    if (self->flags & GL_MAP_PERSISTENT_BIT)
        state = "PERSISTENT";
    else
        state = self->dataPtr != NULL ? "True" : "False";

    return PyUnicode_FromFormat(
        "<%s [size: %d, current offset: %d, mapped: %s] at %X>",
        Py_TYPE(self)->tp_name,
        self->size,
        self->currentOffset,
        state,
        (uintptr_t)self);
}

static PyObject* py_mappedbuffer_map(PyBufferBase* self, PyObject* args, PyObject* kwargs)
{
    static char* kwNames[] = {"offset", "size", NULL};

    GLintptr offset = 0;
    GLsizeiptr mapSize = self->size;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|LL", kwNames, &offset, &mapSize))
        return NULL;

    PyAssert(
        mapSize + offset > self->size,
        "Tried to map buffer above its size.",
        NULL);

    if (!(self->flags & GL_MAP_PERSISTENT_BIT))
    {
        if (self->dataPtr != NULL)
            unmap_gl_buffer(self->id);

        self->dataPtr = map_gl_buffer(self->id, offset, mapSize, self->flags);
    }

    Py_RETURN_NONE;
}

static PyObject* py_mappedbuffer_unmap(PyBufferBase* self, PyObject* Py_UNUSED(args))
{
    // buffer not mapped and is not persistent
    if (self->dataPtr != NULL && !(self->flags & GL_MAP_PERSISTENT_BIT))
    {
        unmap_gl_buffer(self->id);
        self->dataPtr = NULL;
        self->currentOffset = 0;
    }

    Py_RETURN_NONE;
}

static PyObject* py_mappedbuffer_store(PyBufferBase* self, PyObject* data)
{
    ThrowIf(
        self->dataPtr == NULL,
        PyExc_RuntimeError,
        "Buffer has to be mapped before transferring data into it.",
        NULL);

    return store_to_buffer(self, data) ? Py_NewRef(Py_None) : NULL;
}

static PyObject* py_mappedbuffer_reset_offset(PyBufferBase* self, PyObject* Py_UNUSED(args))
{
    self->currentOffset = 0;
    Py_RETURN_NONE;
}

static PyMethodDef pyMappedBufferMethods[] = {
    {"delete", py_mappedbuffer_delete, METH_NOARGS, NULL},
    {"map", py_mappedbuffer_map, METH_VARARGS | METH_KEYWORDS, NULL},
    {"unmap", py_mappedbuffer_unmap, METH_NOARGS, NULL},
    {"store", py_mappedbuffer_store, METH_O, NULL},
    {"reset_offset", py_mappedbuffer_reset_offset, METH_NOARGS, NULL},
    // {"read", py_mappedbuffer_read, METH_VARARGS, NULL},
    {0},
};

PyTypeObject pyMappedBufferType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_new = PyType_GenericNew,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_name = "pygl.buffers.MappedBuffer",
    .tp_basicsize = sizeof(PyBufferBase),
    .tp_init = py_mappedbuffer_init,
    .tp_dealloc = py_mappedbuffer_dealloc,
    .tp_repr = py_mappedbuffer_repr,
    .tp_members = pyBufferBaseMembers,
    .tp_methods = pyMappedBufferMethods,
};
