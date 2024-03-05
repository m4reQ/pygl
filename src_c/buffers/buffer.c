#include "buffer.h"
#include <structmember.h>
#include "../utility.h"

static PyObject* map(PyBuffer* self, PyObject* Py_UNUSED(args))
{
    if (self->flags & GL_MAP_PERSISTENT_BIT)
        Py_RETURN_NONE;

    ThrowIf(
        self->flags & GL_DYNAMIC_STORAGE_BIT,
        PyExc_RuntimeError,
        "Dynamic storage buffers cannot be mapped.",
        NULL);

    ThrowIf(
        self->dataPtr,
        PyExc_RuntimeError,
        "Buffer is already mapped.",
        NULL);

    self->dataPtr = glMapNamedBufferRange(self->id, 0, self->size, self->flags);
    if (!self->dataPtr)
    {
        PyErr_Format(PyExc_RuntimeError, "Couldn't map buffer: %d.", glGetError());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject* transfer(PyBuffer* self, PyObject* Py_UNUSED(args))
{
    // for persistent buffer this function just resets data offset

    if (self->flags & GL_DYNAMIC_STORAGE_BIT)
        glNamedBufferSubData(self->id, 0, (GLsizeiptr)self->currentOffset, self->dataPtr);
    else if (!(self->flags & GL_MAP_PERSISTENT_BIT) && self->dataPtr) // if buffer was not mapped ealier just do nothing
    {
        glUnmapNamedBuffer(self->id);
        self->dataPtr = NULL;
    }

    self->currentOffset = 0;

    Py_RETURN_NONE;
}

static PyObject* store(PyBuffer* self, PyObject* data)
{
    PyObject* result = NULL;

    if (!(self->flags & GL_MAP_PERSISTENT_BIT) &&
        !(self->flags & GL_DYNAMIC_STORAGE_BIT) &&
        !self->dataPtr)
    {
        PyErr_SetString(PyExc_RuntimeError, "Non-persistent buffer has to be mapped prior to storing data.");
        return NULL;
    }

    Py_buffer dataBuffer = {0};
    int _result = PyObject_GetBuffer(data, &dataBuffer, PyBUF_C_CONTIGUOUS);
    ThrowIfGoto(
        _result == -1,
        PyExc_ValueError,
        "Data buffer has to be C-contiguous. For more informations go to https://github.com/m4reQ/pygl?tab=readme-ov-file#buffer-protocol-usage.",
        end);

    ThrowIfGoto(
        dataBuffer.len > self->size - self->currentOffset,
        PyExc_RuntimeError,
        "Data transfer would cause buffer overflow.",
        end);

    _result = PyBuffer_ToContiguous((char*)self->dataPtr + self->currentOffset, &dataBuffer, dataBuffer.len, 'C');
    ThrowIfGoto(
        _result == -1,
        PyExc_RuntimeError,
        "Couldn't transfer data to the buffer.",
        end);

    self->currentOffset += dataBuffer.len;
    result = Py_NewRef(Py_None);

end:
    if (dataBuffer.buf != NULL)
        PyBuffer_Release(&dataBuffer);

    return result;
}

static PyObject* delete(PyBuffer* self, PyObject* Py_UNUSED(args))
{
    glDeleteBuffers(1, &self->id);
    self->id = 0;

    Py_RETURN_NONE;
}

static PyObject* reset_offset(PyBuffer* self, PyObject* Py_UNUSED(args))
{
    self->currentOffset = 0;
    Py_RETURN_NONE;
}

static PyObject* bind(PyBuffer* self, PyObject* target)
{
    if (!PyLong_Check(target))
    {
        PyErr_SetString(PyExc_TypeError, "target has to be of type int.");
        return NULL;
    }

    glBindBuffer(PyLong_AsUnsignedLong(target), self->id);

    Py_RETURN_NONE;
}

static PyObject* bind_base(PyBuffer* self, PyObject* args)
{
    GLenum target = 0;
    GLuint index = 0;
    if (!PyArg_ParseTuple(args, "II", &target, &index))
        return NULL;

    glBindBufferBase(target, index, self->id);

    Py_RETURN_NONE;
}

static PyObject* repr(PyBuffer* self)
{
    return PyUnicode_FromFormat(
        "<%s [size: %d, current offset: %d, memory: %s] at 0x%zx>",
        Py_TYPE(self)->tp_name,
        self->size,
        self->currentOffset,
        (self->flags & GL_DYNAMIC_STORAGE_BIT) ? "CLIENT" : "OPENGL",
        (uintptr_t)self);
}

static PyObject* buf_read(PyBuffer* self, PyObject* args, PyObject* kwargs)
{
    static char* kwNames[] = {"out", "size", "offset", NULL};

    PyObject* result = NULL;

    Py_buffer outBuffer = {0};
    GLsizeiptr size = 0;
    GLintptr offset = 0;
    if (!PyArg_ParseTupleAndKeywords(
        args, kwargs, "w*L|L", kwNames,
        &outBuffer, &size, &offset))
        goto end;

    if (!utils_check_buffer_contiguous(&outBuffer))
        goto end;

    ThrowIfGoto(
        size + offset > self->size,
        PyExc_ValueError,
        "Requested size and offset exceed buffer size.",
        end);

    if (size > outBuffer.len)
    {
        PyErr_Format(PyExc_ValueError, "Buffer that was passed in is smaller than requested size. (buffer size: %zu, requested: %zu)", outBuffer.len, size);
        goto end;
    }

    if (self->flags & GL_DYNAMIC_STORAGE_BIT)
        glGetNamedBufferSubData(self->id, offset, size, outBuffer.buf);
    else
    {
        ThrowIfGoto(
            !(self->flags & GL_MAP_READ_BIT),
            PyExc_RuntimeError,
            "Mappable buffer has to have flag MAP_READ_BIT set to allow reading.",
            end);

        ThrowIfGoto(
            self->dataPtr == NULL,
            PyExc_RuntimeError,
            "Mappable buffer has to be mapped before attempting to read data. Map buffer or use MAP_PERSISTENT_BIT.",
            end);

        int result = PyBuffer_FromContiguous(&outBuffer, (char*)self->dataPtr + offset, size, 'C');
        ThrowIfGoto(
            result == -1,
            PyExc_RuntimeError,
            "Couldn't read data from buffer.",
            end);
    }

    result = Py_NewRef(Py_None);

end:
    if (outBuffer.buf != NULL)
        PyBuffer_Release(&outBuffer);

    return result;
}

static void dealloc(PyBuffer* self)
{
    delete(self, NULL);
    if ((self->flags & GL_DYNAMIC_STORAGE_BIT) && self->dataPtr != NULL)
        PyMem_Free(self->dataPtr);

    Py_TYPE(self)->tp_free(self);
}

static int init(PyBuffer* self, PyObject* args, PyObject* Py_UNUSED(kwargs))
{
    self->currentOffset = 0;
    self->dataPtr = NULL;

    if (!PyArg_ParseTuple(args, "II", &self->size, &self->flags))
        return -1;

    if (self->flags & GL_MAP_PERSISTENT_BIT)
    {
        if (!(self->flags & GL_MAP_WRITE_BIT) && !(self->flags & GL_MAP_READ_BIT))
        {
            PyErr_SetString(PyExc_ValueError, "When using BufferFlags.MAP_PERSISTENT_BIT either BufferFlags.MAP_WRITE_BIT or BufferFlags.MAP_READ_BIT must also be set.");
            return -1;
        }
    }

    glCreateBuffers(1, &self->id);
    glNamedBufferStorage(self->id, self->size, NULL, self->flags);

    if (self->flags & GL_MAP_PERSISTENT_BIT)
        self->dataPtr = glMapNamedBufferRange(self->id, 0, self->size, self->flags);
    else if (self->flags & GL_DYNAMIC_STORAGE_BIT)
        self->dataPtr = PyMem_Malloc(self->size);

    return 0;
}

PyTypeObject pyBufferType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_new = PyType_GenericNew,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_name = "pygl.buffers.Buffer",
    .tp_basicsize = sizeof(PyBuffer),
    .tp_init = (initproc)init,
    .tp_dealloc = (destructor)dealloc,
    .tp_repr = (reprfunc)repr,
    .tp_members = (PyMemberDef[]){
        {"id", T_UINT, offsetof(PyBuffer, id), READONLY, NULL},
        {"size", T_ULONGLONG, offsetof(PyBuffer, size), READONLY, NULL},
        {"current_offset", T_ULONGLONG, offsetof(PyBuffer, currentOffset), 0, NULL},
        {0}},
    .tp_methods = (PyMethodDef[]){
        {"delete", (PyCFunction)delete, METH_NOARGS, NULL},
        {"store", (PyCFunction)store, METH_O, NULL},
        {"transfer", (PyCFunction)transfer, METH_NOARGS, NULL},
        {"reset_offset", (PyCFunction)reset_offset, METH_NOARGS, NULL},
        {"map", (PyCFunction)map, METH_NOARGS, NULL},
        {"read", (PyCFunction)buf_read, METH_VARARGS | METH_KEYWORDS, NULL},
        {0}},
};
