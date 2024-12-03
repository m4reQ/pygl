#pragma once
#include <stdbool.h>
#include <Python.h>
#include <structmember.h>
#include <cglm/mat3.h>
#include <cglm/mat4.h>
// NOTE cglm/mat2 MUST be included after anything that provides cglm_vec4_ucopy, because it
// doesn't include it itself (this is a cglm bug).
#include <cglm/mat2.h>

#pragma pack(push, 1)
typedef struct
{
    PyObject_HEAD
        size_t length;
} Matrix;
#pragma pack(pop)

typedef struct
{
    PyObject_HEAD
        size_t length;
    mat2 data;
} Matrix2;

typedef struct
{
    PyObject_HEAD
        size_t length;
    mat3 data;
} Matrix3;

typedef struct
{
    PyObject_HEAD
        size_t length;
    mat4 data;
} Matrix4;

extern PyTypeObject pyMatrix2Type;
extern PyTypeObject pyMatrix3Type;
extern PyTypeObject pyMatrix4Type;

bool py_matrix_is_matrix(PyObject *obj);
void py_matrix_copy(void *dst, const Matrix *matrix);
uint8_t py_matrix_size(const Matrix *matrix);
