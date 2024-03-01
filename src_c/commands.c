#include <Python.h>
#include <glad/gl.h>
#include "utility.h"
#include "enum.h"

// GL.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA)
// GL.glDepthFunc(GL.GL_LEQUAL)
// GL.glPolygonMode(GL.GL_FRONT_AND_BACK, GL.GL_FILL)
// GL.glPointSize(4.0)
// GL.glClearColor(0.0, 0.0, 0.0, 1.0)

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
    {"cull_face", py_cull_face, METH_O, NULL},
    {"front_face", py_front_face, METH_O, NULL},
    {"enable", py_enable, METH_O, NULL},
    {"hint", py_hint, METH_VARARGS, NULL},
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

    return mod;
}
