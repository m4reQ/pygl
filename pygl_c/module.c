#include "module.h"
#include <stdbool.h>

static bool add_enum(PyObject *module, const EnumDef *enumDef)
{
    static PyObject *enumConstructor = NULL;
    static PyObject *flagConstructor = NULL;

    if (!enumConstructor || !flagConstructor)
    {
        PyObject *enumModule = PyImport_ImportModule("enum");
        enumConstructor = PyObject_GetAttrString(enumModule, "IntEnum");
        flagConstructor = PyObject_GetAttrString(enumModule, "IntFlag");
    }

    PyObject *valuesDict = PyDict_New();

    for (EnumValue *value = enumDef->values; value->name != NULL; value++)
        PyDict_SetItemString(valuesDict, value->name, PyLong_FromUnsignedLong(value->value));

    PyObject *result = PyObject_CallFunction(
        enumDef->isFlag ? flagConstructor : enumConstructor,
        "sN",
        enumDef->enumName,
        valuesDict);
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

    if (info->types)
    {
        PyTypeObject **type = info->types;
        while (*type != NULL)
        {
            if (PyModule_AddType(module, *type))
                return NULL;

            type++;
        }
    }

    if (info->enums)
    {
        EnumDef **enumDef = info->enums;
        while (*enumDef != NULL)
        {
            if (!add_enum(module, *enumDef))
                return NULL;

            enumDef++;
        }
    }

    return module;
}
