#pragma once
#include <Python.h>
#include <structmember.h>
#include <glad/gl.h>
#include "../gl.h"

typedef struct
{
    PyObject_HEAD
    GL_OBJECT_HEAD;
} PyVertexArray;

extern PyTypeObject pyVertexArrayType;
