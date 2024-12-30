#include "matrix.h"
#include <string.h>

bool PyMatrix_Check(PyObject *obj)
{
    return PyObject_IsInstance(obj, (PyObject *)&pyMatrix2Type) ||
           PyObject_IsInstance(obj, (PyObject *)&pyMatrix3Type) ||
           PyObject_IsInstance(obj, (PyObject *)&pyMatrix4Type);
}

void *PyMatrix_GetData(PyObject *matrix)
{
    // NOTE I hope just using offsetof from the biggest matrix type is enough to safely get data pointer
    return (char *)matrix + offsetof(Matrix4, data);
}
