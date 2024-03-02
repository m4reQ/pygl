#pragma once
#include <stdbool.h>
#include <Python.h>

#define ThrowIf(cond, err, msg, ...) do { if (cond) { PyErr_SetString(err, msg); return __VA_ARGS__; } } while (0)
#define PyAssert(cond, msg, ...) do { if (!Py_OptimizeFlag) ThrowIf(!(cond), PyExc_AssertionError, msg, ##__VA_ARGS__); } while (0)

#define _Log(logger, meth, msg, ...) do { PyObject* _msg = PyUnicode_FromFormat(msg, ##__VA_ARGS__); PyObject_CallMethod(logger, meth, "N", msg); } while (0);
#define LogDebug(logger, msg, ...) _Log(logger, "debug", msg, ##__VA_ARGS__)
#define LogInfo(logger, msg, ...) _Log(logger, "info", msg, ##__VA_ARGS__)
#define LogWarning(logger, msg, ...) _Log(logger, "warning", msg, ##__VA_ARGS__)

bool utils_check_buffer_contiguous(const Py_buffer* buf);
