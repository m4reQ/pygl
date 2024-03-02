#include "shader.h"
#include "shaderStage.h"
#include "../enum.h"

static PyModuleDef moduleDef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "pygl.shaders",
};

static EnumValue shaderTypeValues[] = {
    {"FRAGMENT_SHADER", GL_FRAGMENT_SHADER},
    {"VERTEX_SHADER", GL_VERTEX_SHADER},
    {"GEOMETRY_SHADER", GL_GEOMETRY_SHADER},
    {"COMPUTE_SHADER", GL_COMPUTE_SHADER},
    {"TESS_CONTROL_SHADER", GL_TESS_CONTROL_SHADER},
    {"TESS_EVALUATION_SHADER", GL_TESS_EVALUATION_SHADER},
    {0},
};
static EnumValue uniformTypeValues[] = {
    {"FLOAT", GL_FLOAT},
    {"DOUBLE", GL_DOUBLE},
    {"INT", GL_INT},
    {"UNSIGNED_INT", GL_UNSIGNED_INT},
    {0},
};

PyMODINIT_FUNC PyInit_shaders()
{
    PyObject* mod = PyModule_Create(&moduleDef);
    if (!mod)
        return mod;

    if (PyModule_AddType(mod, &pyShaderStageType))
        return NULL;

    if (PyModule_AddType(mod, &pyShaderType))
        return NULL;

    if (!enum_add(mod, "ShaderType", shaderTypeValues))
        return NULL;

    if (!enum_add(mod, "UniformType", uniformTypeValues))
        return NULL;

    return mod;
}
