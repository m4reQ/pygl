#pragma once
#include <Python.h>
#include "../gl.h"

typedef struct
{
    PyObject_HEAD
    GL_OBJECT_HEAD;
    PyObject *resources;
} PyShader;

extern PyTypeObject pyShaderType;
