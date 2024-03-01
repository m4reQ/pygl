#pragma once
#include <Python.h>
#include <structmember.h>
#include <glad/gl.h>

typedef struct
{
    PyObject_HEAD
    GLuint id;
} PyVertexArray;

extern PyTypeObject pyVertexArrayType;
