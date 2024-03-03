#pragma once
#include <Python.h>
#include <glad/gl.h>
#include <stdbool.h>

typedef struct
{
    PyObject_HEAD
    GLuint id;
    void* dataPtr;
    GLenum flags;
    GLsizeiptr size, currentOffset;
} PyBuffer;

extern PyTypeObject pyBufferType;
