#pragma once
#include <stdbool.h>
#include "../gl.h"

typedef struct
{
    PyObject_HEAD
    GL_OBJECT_HEAD;
    void *dataPtr;
    GLenum flags;
    GLsizeiptr size, currentOffset;
    bool isPersistent;
} PyBuffer;

extern PyTypeObject pyBufferType;
