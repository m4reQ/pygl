#pragma once
#include <Python.h>
#include <stdbool.h>

typedef struct
{
    const char* name;
    unsigned int value;
} EnumValue;

bool enum_add(PyObject* module, const char* enumName, EnumValue* values);
