#include "enum.h"

bool enum_add(PyObject* module, const char* enumName, EnumValue* values)
{
    static PyObject* enumConstructor = NULL;
    if (!enumConstructor)
    {
        enumConstructor = PyImport_ImportModule("enum");
        if (!enumConstructor)
        {
            PyErr_SetString(PyExc_ImportError, "Couldn't import enum module.");
            return false;
        }

        enumConstructor = PyObject_GetAttrString(enumConstructor, "IntEnum");
        if (!enumConstructor)
        {
            PyErr_SetString(PyExc_AttributeError, "Couldn't get IntEnum constructor from enum module.");
            return false;
        }
    }

    PyObject* valuesDict = PyDict_New();

    for (EnumValue* value = values; value->name != NULL; value++)
        PyDict_SetItemString(valuesDict, value->name, PyLong_FromUnsignedLong(value->value));

    PyObject* result = PyObject_CallFunction(enumConstructor, "sN", enumName, valuesDict);
    if (!result)
        return false;

    if (PyModule_AddObject(module, enumName, result))
        return false;

    return true;
}
