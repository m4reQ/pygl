#pragma once
#include <Python.h>
#include <structmember.h>
#include <cglm/quat.h>

typedef struct
{
    PyObject_HEAD
    versor data;
} Quaternion;

extern PyTypeObject pyQuaternionType;

void py_quaternion_copy(void *dst, Quaternion *quaternion);
