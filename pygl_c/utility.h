#pragma once
#include <stdbool.h>
#include <Python.h>

#define CAT_NX(A, B) A##B
#define CAT(A, B) CAT_NX(A, B)

#define STRINGIFY_NX(A) #A
#define STRINGIFY(A) STRINGIFY_NX(A)

#define THROW_IF(cond, err, msg, ...)  \
    do                                 \
    {                                  \
        if (cond)                      \
        {                              \
            PyErr_SetString(err, msg); \
            return __VA_ARGS__;        \
        }                              \
    } while (0)
#define THROW_IF_GOTO(cond, err, msg, label) \
    do                                       \
    {                                        \
        if (cond)                            \
        {                                    \
            PyErr_SetString(err, msg);       \
            goto label;                      \
        }                                    \
    } while (0)
#define ASSERT_PY(cond, msg, ...)                                        \
    do                                                                   \
    {                                                                    \
        if (!Py_OptimizeFlag)                                            \
            THROW_IF(!(cond), PyExc_AssertionError, msg, ##__VA_ARGS__); \
    } while (0)

#define LOG_PY(logger, meth, msg, ...)                             \
    do                                                             \
    {                                                              \
        PyObject *_msg = PyUnicode_FromFormat(msg, ##__VA_ARGS__); \
        PyObject_CallMethod(logger, meth, "N", msg);               \
    } while (0);
#define LOG_PY_DEBUG(logger, msg, ...) LOG_PY(logger, "debug", msg, ##__VA_ARGS__)
#define LOG_PY_INFO(logger, msg, ...) LOG_PY(logger, "info", msg, ##__VA_ARGS__)
#define LOG_PY_WARNING(logger, msg, ...) LOG_PY(logger, "warning", msg, ##__VA_ARGS__)

#define FLAG_IS_SET(flags, x) (((flags) & (x)) == (x))
#define FLAG_SET(flags, x) flags |= (x)
#define FLAG_CLEAR(flags, x) flags &= ~(x)

bool utils_check_buffer_contiguous(const Py_buffer *buf);
void raise_buffer_not_contiguous(void);
