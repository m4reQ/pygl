#pragma once
#include <stdbool.h>
#include <Python.h>
#include <structmember.h>
#include <cglm/mat3.h>
#include <cglm/mat4.h>
// NOTE cglm/mat2 MUST be included after anything that provides cglm_vec4_ucopy, because it
// doesn't include it itself (this is a cglm bug).
#include <cglm/mat2.h>

#define PyMatrix_HEAD PyObject_HEAD size_t length

typedef struct
{
    PyMatrix_HEAD;
    mat2 data;
} Matrix2;

typedef struct
{
    PyMatrix_HEAD;
    mat3 data;
} Matrix3;

typedef struct
{
    PyMatrix_HEAD;
    mat4 data;
} Matrix4;

extern PyTypeObject pyMatrix2Type;
extern PyTypeObject pyMatrix3Type;
extern PyTypeObject pyMatrix4Type;

bool PyMatrix_Check(PyObject *obj);
void *PyMatrix_GetData(PyObject *matrix);
