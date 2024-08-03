#include "vector.h"
#include <string.h>

bool py_vector_is_vector(PyObject *obj)
{
    return PyObject_IsInstance(obj, (PyObject *)&pyVector2Type) ||
           PyObject_IsInstance(obj, (PyObject *)&pyVector3Type) ||
           PyObject_IsInstance(obj, (PyObject *)&pyVector4Type);
}

void py_vector_copy(void *dst, const Vector *vector)
{
    memcpy(dst, (char *)vector + sizeof(Vector), py_vector_size(vector));
}

uint8_t py_vector_size(const Vector *vector)
{
    return vector->length * sizeof(float);
}
