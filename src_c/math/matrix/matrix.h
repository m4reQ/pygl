#pragma once
#include <Python.h>
#include <structmember.h>
#include <cglm/mat3.h>
#include <cglm/mat4.h>
// NOTE cglm/mat2 MUST be included after anything that provides cglm_vec4_ucopy, because it
// doesn't include it itself (this is a cglm bug).
#include <cglm/mat2.h>

typedef struct
{
    PyObject_HEAD
        mat2 data;
} Matrix2;

typedef struct
{
    PyObject_HEAD
        mat3 data;
} Matrix3;

typedef struct
{
    PyObject_HEAD
        mat4 data;
} Matrix4;

extern PyTypeObject pyMatrix2Type;
extern PyTypeObject pyMatrix3Type;
extern PyTypeObject pyMatrix4Type;
