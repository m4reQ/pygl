#include "bufferBase.h"
#include "buffer.h"
#include "mappedBuffer.h"
#include "../utility.h"

GLuint create_gl_buffer(GLsizeiptr size, GLenum flags)
{
    GLuint buf = 0;
    glCreateBuffers(1, &buf);
    glNamedBufferStorage(buf, size, NULL, flags);

    return buf;
}

void delete_gl_buffer(GLuint buf)
{
    glDeleteBuffers(1, &buf);
}

void* map_gl_buffer(GLuint buf, GLintptr offset, GLsizeiptr size, GLenum flags)
{
    return glMapNamedBufferRange(buf, offset, size, flags);
}

void unmap_gl_buffer(GLuint buf)
{
    glUnmapNamedBuffer(buf);
}

bool store_to_buffer(PyBufferBase* buf, PyObject* data)
{
    Py_buffer dataBuffer = {0};
    int result = PyObject_GetBuffer(data, &dataBuffer, PyBUF_C_CONTIGUOUS | PyBUF_SIMPLE);
    ThrowIf(
        result == -1,
        PyExc_ValueError,
        "Data must be an object that supports buffer protocol, with c-contiguity and simple layout",
        false);

    ThrowIf(
        dataBuffer.len > buf->size - buf->currentOffset,
        PyExc_RuntimeError,
        "Data transfer would cause buffer overflow.",
        false);

    result = PyBuffer_ToContiguous((char*)buf->dataPtr + buf->currentOffset, &dataBuffer, dataBuffer.len, 'C');
    ThrowIf(
        result == -1,
        PyExc_RuntimeError,
        "Couldn't transfer data to the buffer.",
        false);

    buf->currentOffset += dataBuffer.len;
    PyBuffer_Release(&dataBuffer);

    return true;
}

bool is_buffer(PyObject* obj)
{
    return PyObject_IsInstance(obj, &pyBufferType) || PyObject_IsInstance(obj, &pyMappedBufferType);
}

PyMemberDef pyBufferBaseMembers[] = {
    {"id", T_UINT, offsetof(PyBufferBase, id), READONLY, NULL},
    {"size", T_ULONGLONG, offsetof(PyBufferBase, size), READONLY, NULL},
    {"current_offset", T_ULONGLONG, offsetof(PyBufferBase, currentOffset), 0, NULL},
    {0},
};
