#pragma once
#include <Python.h>
#include <glad/gl.h>

typedef struct
{
    PyObject_HEAD
    GLuint bufferId;
    GLsizeiptr size;
    GLintptr currentOffset;
    void* dataPtr;
    GLenum flags;
    GLuint textureId;
} PyTextureBuffer;

extern PyTypeObject pyTextureBufferType;
