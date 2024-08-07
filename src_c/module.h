#pragma once
#include <stdbool.h>
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
    bool isFlag;
} EnumDef;

typedef struct
{
    PyModuleDef def;
    EnumDef **enums;
    PyTypeObject **types;
} ModuleInfo;

PyObject *module_create_from_info(ModuleInfo *info);
