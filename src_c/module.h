#pragma once
#include <Python.h>

typedef struct
{
    const char *name;
    unsigned int value;
} EnumValue;

typedef struct
{
    const char *enumName;
    EnumValue *values;
} EnumDef;

typedef struct
{
    PyModuleDef def;
    EnumDef **enums;
    PyTypeObject **types;
} ModuleInfo;

PyObject *module_create_from_info(ModuleInfo *info);
