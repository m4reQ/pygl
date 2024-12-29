#include "buffer.h"
#include "../utility.h"
#include "../math/matrix/matrix.h"
#include "../math/vector/vector.h"
#include "../math/quaternion.h"

static PyObject *map(PyBuffer *self, PyObject *Py_UNUSED(args))
{
    if (self->flags & GL_MAP_PERSISTENT_BIT)
        Py_RETURN_NONE;

    THROW_IF(
        self->flags & GL_DYNAMIC_STORAGE_BIT,
        PyExc_RuntimeError,
        "Dynamic storage buffers cannot be mapped.",
        NULL);

    THROW_IF(
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

static PyObject *transfer(PyBuffer *self, PyObject *Py_UNUSED(args))
{
    // for persistent buffer this function just resets data offset

    if (self->flags & GL_DYNAMIC_STORAGE_BIT)
        glNamedBufferSubData(self->id, 0, (GLsizeiptr)self->currentOffset, self->dataPtr);
    else if (!self->isPersistent && self->dataPtr != NULL) // non-persistent buffer that was mapped before
    {
        GLboolean unmapSuccess = glUnmapNamedBuffer(self->id);
        if (!unmapSuccess)
        {
            PyErr_Format(PyExc_RuntimeError, "Failed to unmap buffer: 0x%x.", glGetError());
            return NULL;
        }

        self->dataPtr = NULL;
    }

    self->currentOffset = 0;

    Py_RETURN_NONE;
}

static PyObject *store_address(PyBuffer *self, PyObject *args, PyObject *Py_UNUSED(kwargs))
{
    if (!(self->flags & GL_MAP_PERSISTENT_BIT) &&
        !(self->flags & GL_DYNAMIC_STORAGE_BIT) &&
        !self->dataPtr)
    {
        PyErr_SetString(PyExc_RuntimeError, "Non-persistent buffer has to be mapped prior to storing data.");
        return NULL;
    }

    void *dataBuffer = NULL;
    size_t dataSize = 0;

    if (!PyArg_ParseTuple(args, "nn", &dataBuffer, &dataSize))
        return NULL;

    if (dataBuffer == NULL || dataSize == 0)
        Py_RETURN_NONE;

    THROW_IF(
        dataSize > self->size - self->currentOffset,
        PyExc_TypeError,
        "Data transfer would cause buffer overflow.",
        NULL);

    memcpy((char *)self->dataPtr + self->currentOffset, dataBuffer, dataSize);
    self->currentOffset += dataSize;

end:
    Py_RETURN_NONE;
}

static PyObject *store(PyBuffer *self, PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {"data", "offset", NULL};

    PyObject *result = NULL;

    if (!(self->flags & GL_MAP_PERSISTENT_BIT) &&
        !(self->flags & GL_DYNAMIC_STORAGE_BIT) &&
        !self->dataPtr)
    {
        PyErr_SetString(PyExc_RuntimeError, "Non-persistent buffer has to be mapped prior to storing data.");
        return NULL;
    }

    PyObject *data = NULL;
    Py_buffer dataBuffer = {0};
    size_t offset = self->currentOffset;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|n", kwNames, &data, &offset))
        return NULL;

    void *dataPtr = (char *)self->dataPtr + offset;
    size_t dataSize = 0;

    // FIXME Add fast path for matrix and vector stores
    if (PyObject_CheckBuffer(data))
    {
        int _result = PyObject_GetBuffer(data, &dataBuffer, PyBUF_CONTIG_RO); // double check to catch non-contiguous buffers
        THROW_IF_GOTO(
            _result == -1,
            PyExc_ValueError,
            "Data buffer has to be C-contiguous. For more informations go to https://github.com/m4reQ/pygl?tab=readme-ov-file#buffer-protocol-usage.",
            end);

        THROW_IF_GOTO(
            dataBuffer.len > self->size - self->currentOffset,
            PyExc_RuntimeError,
            "Data transfer would cause buffer overflow.",
            end);

        memcpy(dataPtr, dataBuffer.buf, dataBuffer.len);
        dataSize = dataBuffer.len;
    }
    // else if (PyObject_IsInstance(data, (PyObject *)&pyQuaternionType))
    // {
    //     py_quaternion_copy(dataPtr, (Quaternion *)data);
    //     dataSize = sizeof(versor);
    // }
    else if (PyLong_Check(data))
    {
        *(int *)dataPtr = PyLong_AsLong(data);
        dataSize = sizeof(int);
    }
    else if (PyFloat_Check(data))
    {
        *(float *)dataPtr = (float)PyFloat_AS_DOUBLE(data);
        dataSize = sizeof(float);
    }
    else
    {
        PyErr_Format(PyExc_TypeError, "Expected argument to be of math type, buffer or simple numeric type, got %s.", Py_TYPE(data)->tp_name);
        return NULL;
    }

    if (offset == 0 || offset == self->currentOffset)
        self->currentOffset += dataSize;

    result = Py_NewRef(Py_None);

end:
    if (dataBuffer.buf != NULL)
        PyBuffer_Release(&dataBuffer);

    return result;
}

static PyObject *delete(PyBuffer *self, PyObject *Py_UNUSED(args))
{
    glDeleteBuffers(1, &self->id);
    self->id = 0;

    Py_RETURN_NONE;
}

static PyObject *reset_offset(PyBuffer *self, PyObject *Py_UNUSED(args))
{
    self->currentOffset = 0;
    Py_RETURN_NONE;
}

static PyObject *bind(PyBuffer *self, PyObject *target)
{
    if (!PyLong_Check(target))
    {
        PyErr_SetString(PyExc_TypeError, "target has to be of type int.");
        return NULL;
    }

    glBindBuffer(PyLong_AsUnsignedLong(target), self->id);

    Py_RETURN_NONE;
}

static PyObject *bind_base(PyBuffer *self, PyObject *args)
{
    GLenum target = 0;
    GLuint index = 0;
    if (!PyArg_ParseTuple(args, "II", &target, &index))
        return NULL;

    glBindBufferBase(target, index, self->id);

    Py_RETURN_NONE;
}

static PyObject *repr(PyBuffer *self)
{
    return PyUnicode_FromFormat(
        "<object %s (size: %d, current offset: %d, memory: %s) at 0x%zX>",
        Py_TYPE(self)->tp_name,
        self->size,
        self->currentOffset,
        (self->flags & GL_DYNAMIC_STORAGE_BIT) ? "CLIENT" : "OPENGL",
        (uintptr_t)self);
}

static PyObject *buf_read(PyBuffer *self, PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {"out", "size", "offset", NULL};

    PyObject *result = NULL;

    Py_buffer outBuffer = {0};
    GLsizeiptr size = 0;
    GLintptr offset = 0;
    if (!PyArg_ParseTupleAndKeywords(
            args, kwargs, "w*L|L", kwNames,
            &outBuffer, &size, &offset))
        goto end;

    if (!utils_check_buffer_contiguous(&outBuffer))
        goto end;

    THROW_IF_GOTO(
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
        THROW_IF_GOTO(
            !(self->flags & GL_MAP_READ_BIT),
            PyExc_RuntimeError,
            "Mappable buffer has to have flag MAP_READ_BIT set to allow reading.",
            end);

        THROW_IF_GOTO(
            self->dataPtr == NULL,
            PyExc_RuntimeError,
            "Mappable buffer has to be mapped before attempting to read data. Map buffer or use MAP_PERSISTENT_BIT.",
            end);

        int result = PyBuffer_FromContiguous(&outBuffer, (char *)self->dataPtr + offset, size, 'C');
        THROW_IF_GOTO(
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

static void dealloc(PyBuffer *self)
{
    delete (self, NULL);
    if ((self->flags & GL_DYNAMIC_STORAGE_BIT) && self->dataPtr != NULL)
        PyMem_Free(self->dataPtr);

    Py_TYPE(self)->tp_free(self);
}

static int init(PyBuffer *self, PyObject *args, PyObject *Py_UNUSED(kwargs))
{
    if (!PyArg_ParseTuple(args, "II", &self->size, &self->flags))
        return -1;

    if (self->flags & GL_MAP_PERSISTENT_BIT)
    {
        if (!(self->flags & GL_MAP_WRITE_BIT) && !(self->flags & GL_MAP_READ_BIT))
        {
            PyErr_SetString(PyExc_ValueError, "When using BufferFlags.MAP_PERSISTENT_BIT either BufferFlags.MAP_WRITE_BIT or BufferFlags.MAP_READ_BIT must also be set.");
            return -1;
        }

        self->isPersistent = true;
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
        {"delete", (PyCFunction) delete, METH_NOARGS, NULL},
        {"store", (PyCFunction)store, METH_VARARGS | METH_KEYWORDS, NULL},
        {"store_address", (PyCFunction)store_address, METH_VARARGS, NULL},
        {"transfer", (PyCFunction)transfer, METH_NOARGS, NULL},
        {"reset_offset", (PyCFunction)reset_offset, METH_NOARGS, NULL},
        {"map", (PyCFunction)map, METH_NOARGS, NULL},
        {"read", (PyCFunction)buf_read, METH_VARARGS | METH_KEYWORDS, NULL},
        {"bind_base", (PyCFunction)bind_base, METH_VARARGS, NULL},
        {"bind", (PyCFunction)bind, METH_O, NULL},
        {0},
    },
};
