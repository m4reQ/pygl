#pragma once
#include <Python.h>
#include <glad/gl.h>
#include <stdbool.h>

typedef struct
{
    PyObject_HEAD
    GLuint attribIndex;
    GLenum type;
    GLint count;
    GLint rows;
    bool isNormalized;
} PyVertexDescriptor;

extern PyTypeObject pyVertexDescriptorType;
