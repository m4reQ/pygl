#include <Python.h>
#include <glad/gl.h>
#include "utility.h"
#include "enum.h"

typedef struct
{
    PyObject* callback;
    PyObject* data;
} CallbackData;

static CallbackData callbackData = {0};

static void free_callback_data(void)
{
    Py_XDECREF(callbackData.callback);
    Py_XDECREF(callbackData.data);

    callbackData = (CallbackData){0};
}

static void opengl_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if (!callbackData.callback)
        return;

    // we cannot viably propagate exceptions that happened during callback execution
    // so to prevent crash just silence them. It might lead to bugs during later execution
    PyObject* result = PyObject_CallFunction(
        callbackData.callback, "IIIIsO",
        source, type, id, severity, message,
        callbackData.data ? callbackData.data : Py_NewRef(Py_None));
    if (result == NULL)
    {
        printf("OpenGL debug callback executed with an exception that will be ignored. For more info go to https://github.com/m4reQ/pygl?tab=readme-ov-file#opengl-debug-callback. \n");
        PyErr_Clear();
    }
}

static PyObject* get_error(PyObject* Py_UNUSED(self), PyObject* Py_UNUSED(args))
{
    return PyLong_FromUnsignedLong(glGetError());
}

static PyObject* py_debug_enable(PyObject* Py_UNUSED(self), PyObject* args, PyObject* kwargs)
{
    static char* kwNames[] = {"callback", "user_data", NULL};

    PyObject* callback = NULL;
    PyObject* data = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O", kwNames, &callback, &data))
        return NULL;

    ThrowIf(
        !PyCallable_Check(callback),
        PyExc_TypeError,
        "callback has to be of type callable.",
        NULL);

    callbackData.callback = Py_NewRef(callback);
    if (data)
        callbackData.data = Py_NewRef(data);

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(opengl_callback, NULL);

    // Issue test message
    const char msg[] = "OpenGL debug output enabled.";
    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_OTHER, 2137, GL_DEBUG_SEVERITY_NOTIFICATION, sizeof(msg), msg);

    Py_RETURN_NONE;
}

static PyObject* py_debug_disable(PyObject* Py_UNUSED(self), PyObject* Py_UNUSED(args))
{
    free_callback_data();

    glDisable(GL_DEBUG_OUTPUT);
    glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    Py_RETURN_NONE;
}

static void py_debug_free(void* Py_UNUSED(self))
{
    free_callback_data();
}

static PyMethodDef moduleMethods[] = {
    {"enable", (PyCFunction)py_debug_enable, METH_VARARGS | METH_KEYWORDS, NULL},
    {"disable", py_debug_disable, METH_NOARGS, NULL},
    {"get_error", get_error, METH_NOARGS, NULL},
    {0},
};

static EnumValue debugSeverityValues[] = {
    {"DEBUG_SEVERITY_NOTIFICATION", GL_DEBUG_SEVERITY_NOTIFICATION},
    {"DEBUG_SEVERITY_LOW", GL_DEBUG_SEVERITY_LOW},
    {"DEBUG_SEVERITY_MEDIUM", GL_DEBUG_SEVERITY_MEDIUM},
    {"DEBUG_SEVERITY_HIGH", GL_DEBUG_SEVERITY_HIGH},
    {0},
};
static EnumValue debugSourceValues[] = {
    {"DEBUG_SOURCE_API", GL_DEBUG_SOURCE_API},
    {"DEBUG_SOURCE_APPLICATION", GL_DEBUG_SOURCE_APPLICATION},
    {"DEBUG_SOURCE_THIRD_PARTY", GL_DEBUG_SOURCE_THIRD_PARTY},
    {"DEBUG_SOURCE_WINDOW_SYSTEM", GL_DEBUG_SOURCE_WINDOW_SYSTEM},
    {"DEBUG_SOURCE_OTHER", GL_DEBUG_SOURCE_OTHER},
    {0},
};
static EnumValue debugTypeValues[] = {
    {"DEBUG_TYPE_DEPRECATED_BEHAVIOR", GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR},
    {"DEBUG_TYPE_MARKER", GL_DEBUG_TYPE_MARKER},
    {"DEBUG_TYPE_OTHER", GL_DEBUG_TYPE_OTHER},
    {"DEBUG_TYPE_UNDEFINED_BEHAVIOR", GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR},
    {"DEBUG_TYPE_PORTABILITY", GL_DEBUG_TYPE_PORTABILITY},
    {"DEBUG_TYPE_POP_GROUP", GL_DEBUG_TYPE_POP_GROUP},
    {"DEBUG_TYPE_PUSH_GROUP", GL_DEBUG_TYPE_PUSH_GROUP},
    {"DEBUG_TYPE_ERROR", GL_DEBUG_TYPE_ERROR},
    {"DEBUG_TYPE_PERFORMANCE", GL_DEBUG_TYPE_PERFORMANCE},
    {0},
};
static EnumValue errorCodeValues[] = {
    {"NO_ERROR", GL_NO_ERROR},
    {"INVALID_ENUM", GL_INVALID_ENUM},
    {"INVALID_VALUE", GL_INVALID_VALUE},
    {"INVALID_OPERATION", GL_INVALID_OPERATION},
    {"INVALID_FRAMEBUFFER_OPERATION", GL_INVALID_FRAMEBUFFER_OPERATION},
    {"OUT_OF_MEMORY", GL_OUT_OF_MEMORY},
    {"STACK_UNDERFLOW", GL_STACK_UNDERFLOW},
    {"STACK_OVERFLOW", GL_STACK_OVERFLOW},
    {0},
};

static PyModuleDef moduleDef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "pygl.debug",
    .m_size = -1,
    .m_methods = moduleMethods,
    .m_free = py_debug_free,
};

PyMODINIT_FUNC PyInit_debug()
{
    PyObject* mod = PyModule_Create(&moduleDef);
    if (!mod)
        return NULL;

    if (!enum_add(mod, "DebugSeverity", debugSeverityValues))
        return NULL;

    if (!enum_add(mod, "DebugSource", debugSourceValues))
        return NULL;

    if (!enum_add(mod, "DebugType", debugTypeValues))
        return NULL;

    if (!enum_add(mod, "ErrorCode", errorCodeValues))
        return NULL;

    return mod;
}
