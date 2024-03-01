#include <Python.h>
#include <glad/gl.h>
#include "utility.h"
#include "enum.h"

static PyObject* py_blend_func(PyObject* Py_UNUSED(self), PyObject* args)
{
    GLenum src = 0, dst = 0;

    if (!PyArg_ParseTuple(args, "II", &src, &dst))
        return NULL;

    glBlendFunc(src, dst);
    Py_RETURN_NONE;
}

static PyObject* py_polygon_mode(PyObject* Py_UNUSED(self), PyObject* args)
{
    GLenum face = 0, mode = 0;

    if (!PyArg_ParseTuple(args, "II", &face, &mode))
        return NULL;

    glPolygonMode(face, mode);
    Py_RETURN_NONE;
}

static PyObject* py_depth_func(PyObject* Py_UNUSED(self), PyObject* func)
{
    ThrowIf(
        !PyLong_Check(func),
        PyExc_TypeError,
        "Depth func has to be of type int.",
        NULL);

    glDepthFunc(PyLong_AsUnsignedLong(func));
    Py_RETURN_NONE;
}

static PyObject* py_point_size(PyObject* Py_UNUSED(self), PyObject* size)
{
    ThrowIf(
        !PyFloat_Check(size),
        PyExc_TypeError,
        "Point size has to be of type float.",
        NULL);

    glPointSize((GLfloat)PyFloat_AS_DOUBLE(size));
    Py_RETURN_NONE;
}

static PyObject* py_cull_face(PyObject* Py_UNUSED(self), PyObject* face)
{
    ThrowIf(
        !PyLong_Check(face),
        PyExc_TypeError,
        "Face has to be of type int.",
        NULL);

    glCullFace(PyLong_AsUnsignedLong(face));

    Py_RETURN_NONE;
}

static PyObject* py_front_face(PyObject* Py_UNUSED(self), PyObject* face)
{
    ThrowIf(
        !PyLong_Check(face),
        PyExc_TypeError,
        "Face has to be of type int.",
        NULL);

    glFrontFace(PyLong_AsUnsignedLong(face));

    Py_RETURN_NONE;
}

static PyObject* py_hint(PyObject* Py_UNUSED(self), PyObject* args)
{
    GLenum hint = 0, value = 0;
    if (!PyArg_ParseTuple(args, "II", &hint, &value))
        return NULL;

    glHint(hint, value);

    Py_RETURN_NONE;
}

static PyObject* py_enable(PyObject* Py_UNUSED(self), PyObject* cap)
{
    ThrowIf(
        !PyLong_Check(cap),
        PyExc_TypeError,
        "Enable cap has to be of type int.",
        NULL);

    glEnable(PyLong_AsUnsignedLong(cap));

    Py_RETURN_NONE;
}

static PyMethodDef moduleMethods[] = {
    {"polygon_mode", py_polygon_mode, METH_VARARGS, NULL},
    {"depth_func", py_depth_func, METH_O, NULL},
    {"point_size", py_point_size, METH_O, NULL},
    {"cull_face", py_cull_face, METH_O, NULL},
    {"front_face", py_front_face, METH_O, NULL},
    {"enable", py_enable, METH_O, NULL},
    {"hint", py_hint, METH_VARARGS, NULL},
    {"blend_func", py_blend_func, METH_VARARGS, NULL},
    {0},
};
static PyModuleDef moduleDef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "pygl.commands",
    .m_methods = moduleMethods,
    .m_size = -1,
};

static EnumValue cullFaces[] = {
    {"FRONT", GL_FRONT},
    {"BACK", GL_BACK},
    {"FRONT_AND_BACK", GL_FRONT_AND_BACK},
    {0},
};
static EnumValue frontFaces[] = {
    {"CW", GL_CW},
    {"CCW", GL_CCW},
    {0},
};
static EnumValue hintValues[] = {
    {"NICEST", GL_NICEST},
    {"FASTEST", GL_FASTEST},
    {"DONT_CARE", GL_DONT_CARE},
    {0},
};
static EnumValue hintTargets[] = {
    {"FRAGMENT_SHADER_DERIVATIVE_HINT", GL_FRAGMENT_SHADER_DERIVATIVE_HINT},
    {"LINE_SMOOTH_HINT", GL_LINE_SMOOTH_HINT},
    {"POLYGON_SMOOTH_HINT", GL_POLYGON_SMOOTH_HINT},
    {"TEXTURE_COMPRESSION_HINT", GL_TEXTURE_COMPRESSION_HINT},
    {0},
};
static EnumValue enableValues[] = {
    {"BLEND", GL_BLEND},
    {"CLIP_DISTANCE0", GL_CLIP_DISTANCE0},
    {"CLIP_DISTANCE1", GL_CLIP_DISTANCE1},
    {"CLIP_DISTANCE2", GL_CLIP_DISTANCE2},
    {"CLIP_DISTANCE3", GL_CLIP_DISTANCE3},
    {"CLIP_DISTANCE4", GL_CLIP_DISTANCE4},
    {"CLIP_DISTANCE5", GL_CLIP_DISTANCE5},
    {"CLIP_DISTANCE6", GL_CLIP_DISTANCE6},
    {"CLIP_DISTANCE7", GL_CLIP_DISTANCE7},
    {"COLOR_LOGIC_OP", GL_COLOR_LOGIC_OP},
    {"CULL_FACE", GL_CULL_FACE},
    {"DEBUG_OUTPUT", GL_DEBUG_OUTPUT},
    {"DEBUG_OUTPUT_SYNCHRONOUS", GL_DEBUG_OUTPUT_SYNCHRONOUS},
    {"DEPTH_CLAMP", GL_DEPTH_CLAMP},
    {"DEPTH_TEST", GL_DEPTH_TEST},
    {"DITHER", GL_DITHER},
    {"FRAMEBUFFER_SRGB", GL_FRAMEBUFFER_SRGB},
    {"LINE_SMOOTH", GL_LINE_SMOOTH},
    {"MULTISAMPLE", GL_MULTISAMPLE},
    {"POLYGON_OFFSET_FILL", GL_POLYGON_OFFSET_FILL},
    {"POLYGON_OFFSET_LINE", GL_POLYGON_OFFSET_LINE},
    {"POLYGON_OFFSET_POINT", GL_POLYGON_OFFSET_POINT},
    {"POLYGON_SMOOTH", GL_POLYGON_SMOOTH},
    {"PRIMITIVE_RESTART", GL_PRIMITIVE_RESTART},
    {"PRIMITIVE_RESTART_FIXED_INDEX", GL_PRIMITIVE_RESTART_FIXED_INDEX},
    {"RASTERIZER_DISCARD", GL_RASTERIZER_DISCARD},
    {"SAMPLE_ALPHA_TO_COVERAGE", GL_SAMPLE_ALPHA_TO_COVERAGE},
    {"SAMPLE_ALPHA_TO_ONE", GL_SAMPLE_ALPHA_TO_ONE},
    {"SAMPLE_COVERAGE", GL_SAMPLE_COVERAGE},
    {"SAMPLE_SHADING", GL_SAMPLE_SHADING},
    {"SAMPLE_MASK", GL_SAMPLE_MASK},
    {"SCISSOR_TEST", GL_SCISSOR_TEST},
    {"STENCIL_TEST", GL_STENCIL_TEST},
    {"TEXTURE_CUBE_MAP_SEAMLESS", GL_TEXTURE_CUBE_MAP_SEAMLESS},
    {"PROGRAM_POINT_SIZE", GL_PROGRAM_POINT_SIZE},
    {0},
};
static EnumValue depthFuncValues[] = {
    {"NEVER", GL_NEVER},
    {"LESS", GL_LESS},
    {"EQUAL", GL_EQUAL},
    {"LEQUAL", GL_LEQUAL},
    {"GREATER", GL_GREATER},
    {"NOTEQUAL", GL_NOTEQUAL},
    {"GEQUAL", GL_GEQUAL},
    {"ALWAYS", GL_ALWAYS},
    {0},
};
static EnumValue polygonModeValues[] = {
    {"FILL", GL_FILL},
    {"POINT", GL_POINT},
    {"LINE", GL_LINE},
    {0},
};
static EnumValue blendFuncValues[] = {
    {"ZERO", GL_ZERO},
    {"ONE", GL_ONE},
    {"SRC_COLOR", GL_SRC_COLOR},
    {"ONE_MINUS_SRC_COLOR", GL_ONE_MINUS_SRC_COLOR},
    {"DST_COLOR", GL_DST_COLOR},
    {"ONE_MINUS_DST_COLOR", GL_ONE_MINUS_DST_COLOR},
    {"SRC_ALPHA", GL_SRC_ALPHA},
    {"ONE_MINUS_SRC_ALPHA", GL_ONE_MINUS_SRC_ALPHA},
    {"DST_ALPHA", GL_DST_ALPHA},
    {"ONE_MINUS_DST_ALPHA", GL_ONE_MINUS_DST_ALPHA},
    {"CONSTANT_COLOR", GL_CONSTANT_COLOR},
    {"ONE_MINUS_CONSTANT_COLOR", GL_ONE_MINUS_CONSTANT_COLOR},
    {"CONSTANT_ALPHA", GL_CONSTANT_ALPHA},
    {"ONE_MINUS_CONSTANT_ALPHA", GL_ONE_MINUS_CONSTANT_ALPHA},
    {0},
};

PyMODINIT_FUNC PyInit_commands()
{
    PyObject* mod = PyModule_Create(&moduleDef);
    if (!mod)
        return NULL;

    if (!enum_add(mod, "EnableCap", enableValues))
        return NULL;

    if (!enum_add(mod, "HintValue", hintValues))
        return NULL;

    if (!enum_add(mod, "HintTarget", hintTargets))
        return NULL;

    if (!enum_add(mod, "FrontFace", frontFaces))
        return NULL;

    if (!enum_add(mod, "CullFace", cullFaces))
        return NULL;

    if (!enum_add(mod, "DepthFunc", depthFuncValues))
        return NULL;

    if (!enum_add(mod, "PolygonMode", polygonModeValues))
        return NULL;

    if (!enum_add(mod, "BlendFactor", blendFuncValues))
        return NULL;

    return mod;
}
