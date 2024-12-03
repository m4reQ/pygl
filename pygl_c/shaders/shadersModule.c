#include "shader.h"
#include "shaderStage.h"
#include "../module.h"

static EnumDef shaderTypeEnum = {
    .enumName = "ShaderType",
    .values = (EnumValue[]) {
        {"FRAGMENT_SHADER", GL_FRAGMENT_SHADER},
        {"VERTEX_SHADER", GL_VERTEX_SHADER},
        {"GEOMETRY_SHADER", GL_GEOMETRY_SHADER},
        {"COMPUTE_SHADER", GL_COMPUTE_SHADER},
        {"TESS_CONTROL_SHADER", GL_TESS_CONTROL_SHADER},
        {"TESS_EVALUATION_SHADER", GL_TESS_EVALUATION_SHADER},
        {0}},
};

static EnumDef uniformTypeEnum = {
    .enumName = "UniformType",
    .values = (EnumValue[]) {
        {"FLOAT", GL_FLOAT},
        {"DOUBLE", GL_DOUBLE},
        {"INT", GL_INT},
        {"UNSIGNED_INT", GL_UNSIGNED_INT},
        {0}},
};

static ModuleInfo modInfo = {
    .def = {
        PyModuleDef_HEAD_INIT,
        .m_name = "pygl.shaders",
        .m_size = -1},
    .enums = (EnumDef*[]) {
        &shaderTypeEnum,
        &uniformTypeEnum,
        NULL},
    .types = (PyTypeObject*[]) {
        &pyShaderStageType,
        &pyShaderType,
        NULL},
};

PyMODINIT_FUNC PyInit_shaders()
{
    return module_create_from_info(&modInfo);
}
