#pragma once
#include "../gl.h"

typedef struct
{
    PyObject_HEAD
    GL_OBJECT_HEAD;
    void *dataPtr;
    GLenum flags;
    GLsizeiptr size, currentOffset;
} PyBuffer;

extern PyTypeObject pyBufferType;
