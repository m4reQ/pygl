#include "module.h"
#include <stdbool.h>

static bool add_enum(PyObject *module, const EnumDef *enumDef)
{
    static PyObject *enumConstructor = NULL;
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

    PyObject *valuesDict = PyDict_New();

    for (EnumValue *value = enumDef->values; value->name != NULL; value++)
        PyDict_SetItemString(valuesDict, value->name, PyLong_FromUnsignedLong(value->value));

    PyObject *result = PyObject_CallFunction(enumConstructor, "sN", enumDef->enumName, valuesDict);
    if (!result)
        return false;

    if (PyModule_AddObject(module, enumDef->enumName, result))
        return false;

    return true;
}

PyObject *module_create_from_info(ModuleInfo *info)
{
    PyObject *module = PyModule_Create(&info->def);
    if (!module)
        return NULL;

    for (PyTypeObject **type = info->types; *type != NULL; type++)
    {
        if (PyModule_AddType(module, *type))
            return NULL;
    }

    if (info->enums)
    {
        for (EnumDef **enumDef = info->enums; *enumDef != NULL; enumDef++)
        {
            if (!add_enum(module, *enumDef))
                return NULL;
        }
    }

    return module;
}
