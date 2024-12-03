#include "matrix.h"
#include <string.h>

bool py_matrix_is_matrix(PyObject *obj)
{
    return PyObject_IsInstance(obj, (PyObject *)&pyMatrix2Type) ||
           PyObject_IsInstance(obj, (PyObject *)&pyMatrix3Type) ||
           PyObject_IsInstance(obj, (PyObject *)&pyMatrix4Type);
}

void py_matrix_copy(void *dst, const Matrix *matrix)
{
    memcpy(dst, (char *)matrix + sizeof(Matrix), py_matrix_size(matrix));
}

uint8_t py_matrix_size(const Matrix *matrix)
{
    return matrix->length * sizeof(float);
}
