#pragma once
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    GLsync sync;
} PySync;

extern PyTypeObject pySyncType;
