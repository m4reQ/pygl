#pragma once
#include <Python.h>
#include <glad/gl.h>
#include "../buffers/buffer.h"

typedef struct
{
    PyObject_HEAD
    GLuint bufferId;
    GLsizei stride;
    PyObject* descriptors; // list of PyVertexDescriptor
    GLintptr offset;
    GLuint divisor;
} PyVertexInput;

extern PyTypeObject pyVertexInputType;
