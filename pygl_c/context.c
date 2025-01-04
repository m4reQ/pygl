#include <Python.h>
#include <glad/gl.h>
#include "utility.h"
#include "module.h"

static PyObject *py_get_integer(PyObject *Py_UNUSED(self), PyObject *name)
{
    if (!PyLong_Check(name))
    {
        PyErr_Format(PyExc_TypeError, "Expected name to be of type int, got: %s", Py_TYPE(name)->tp_name);
        return NULL;
    }

    GLint value = 0;
    glGetIntegerv(PyLong_AsUnsignedLong(name), &value);

    return PyLong_FromLong(value);
}

static PyObject *py_is_enabled(PyObject *Py_UNUSED(self), PyObject *enableCap)
{
    if (!PyLong_Check(enableCap))
    {
        PyErr_Format(PyExc_TypeError, "Expected enable_cap to be of type int, got: %s", Py_TYPE(enableCap)->tp_name);
        return NULL;
    }

    return PyBool_FromLong((long)glIsEnabled(PyLong_AsUnsignedLong(enableCap)));
}

static EnumDef getNameEnum = {
    .enumName = "GetName",
    .values = (EnumValue[]){
        {"ACTIVE_TEXTURE", GL_ACTIVE_TEXTURE},
        {"ALIASED_LINE_WIDTH_RANGE", GL_ALIASED_LINE_WIDTH_RANGE},
        {"ARRAY_BUFFER_BINDING", GL_ARRAY_BUFFER_BINDING},
        {"BLEND", GL_BLEND},
        {"BLEND_COLOR", GL_BLEND_COLOR},
        {"BLEND_DST_ALPHA", GL_BLEND_DST_ALPHA},
        {"BLEND_DST_RGB", GL_BLEND_DST_RGB},
        {"BLEND_EQUATION_RGB", GL_BLEND_EQUATION_RGB},
        {"BLEND_EQUATION_ALPHA", GL_BLEND_EQUATION_ALPHA},
        {"BLEND_SRC_ALPHA", GL_BLEND_SRC_ALPHA},
        {"BLEND_SRC_RGB", GL_BLEND_SRC_RGB},
        {"COLOR_CLEAR_VALUE", GL_COLOR_CLEAR_VALUE},
        {"COLOR_LOGIC_OP", GL_COLOR_LOGIC_OP},
        {"COLOR_WRITEMASK", GL_COLOR_WRITEMASK},
        {"COMPRESSED_TEXTURE_FORMATS", GL_COMPRESSED_TEXTURE_FORMATS},
        {"MAX_COMPUTE_SHADER_STORAGE_BLOCKS", GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS},
        {"MAX_COMBINED_SHADER_STORAGE_BLOCKS", GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS},
        {"MAX_COMPUTE_UNIFORM_BLOCKS", GL_MAX_COMPUTE_UNIFORM_BLOCKS},
        {"MAX_COMPUTE_TEXTURE_IMAGE_UNITS", GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS},
        {"MAX_COMPUTE_UNIFORM_COMPONENTS", GL_MAX_COMPUTE_UNIFORM_COMPONENTS},
        {"MAX_COMPUTE_ATOMIC_COUNTERS", GL_MAX_COMPUTE_ATOMIC_COUNTERS},
        {"MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS", GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS},
        {"MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS", GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS},
        {"MAX_COMPUTE_WORK_GROUP_INVOCATIONS", GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS},
        {"MAX_COMPUTE_WORK_GROUP_COUNT", GL_MAX_COMPUTE_WORK_GROUP_COUNT},
        {"MAX_COMPUTE_WORK_GROUP_SIZE", GL_MAX_COMPUTE_WORK_GROUP_SIZE},
        {"DISPATCH_INDIRECT_BUFFER_BINDING", GL_DISPATCH_INDIRECT_BUFFER_BINDING},
        {"MAX_DEBUG_GROUP_STACK_DEPTH", GL_MAX_DEBUG_GROUP_STACK_DEPTH},
        {"DEBUG_GROUP_STACK_DEPTH", GL_DEBUG_GROUP_STACK_DEPTH},
        {"CONTEXT_FLAGS", GL_CONTEXT_FLAGS},
        {"CULL_FACE", GL_CULL_FACE},
        {"CULL_FACE_MODE", GL_CULL_FACE_MODE},
        {"CURRENT_PROGRAM", GL_CURRENT_PROGRAM},
        {"DEPTH_CLEAR_VALUE", GL_DEPTH_CLEAR_VALUE},
        {"DEPTH_FUNC", GL_DEPTH_FUNC},
        {"DEPTH_RANGE", GL_DEPTH_RANGE},
        {"DEPTH_TEST", GL_DEPTH_TEST},
        {"DEPTH_WRITEMASK", GL_DEPTH_WRITEMASK},
        {"DITHER", GL_DITHER},
        {"DOUBLEBUFFER", GL_DOUBLEBUFFER},
        {"DRAW_BUFFER", GL_DRAW_BUFFER},
        {"DRAW_FRAMEBUFFER_BINDING", GL_DRAW_FRAMEBUFFER_BINDING},
        {"READ_FRAMEBUFFER_BINDING", GL_READ_FRAMEBUFFER_BINDING},
        {"ELEMENT_ARRAY_BUFFER_BINDING", GL_ELEMENT_ARRAY_BUFFER_BINDING},
        {"FRAGMENT_SHADER_DERIVATIVE_HINT", GL_FRAGMENT_SHADER_DERIVATIVE_HINT},
        {"IMPLEMENTATION_COLOR_READ_FORMAT", GL_IMPLEMENTATION_COLOR_READ_FORMAT},
        {"IMPLEMENTATION_COLOR_READ_TYPE", GL_IMPLEMENTATION_COLOR_READ_TYPE},
        {"LINE_SMOOTH", GL_LINE_SMOOTH},
        {"LINE_SMOOTH_HINT", GL_LINE_SMOOTH_HINT},
        {"LINE_WIDTH", GL_LINE_WIDTH},
        {"LAYER_PROVOKING_VERTEX", GL_LAYER_PROVOKING_VERTEX},
        {"LOGIC_OP_MODE", GL_LOGIC_OP_MODE},
        {"MAJOR_VERSION", GL_MAJOR_VERSION},
        {"MAX_3D_TEXTURE_SIZE", GL_MAX_3D_TEXTURE_SIZE},
        {"MAX_ARRAY_TEXTURE_LAYERS", GL_MAX_ARRAY_TEXTURE_LAYERS},
        {"MAX_CLIP_DISTANCES", GL_MAX_CLIP_DISTANCES},
        {"MAX_COLOR_TEXTURE_SAMPLES", GL_MAX_COLOR_TEXTURE_SAMPLES},
        {"MAX_COMBINED_ATOMIC_COUNTERS", GL_MAX_COMBINED_ATOMIC_COUNTERS},
        {"MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS", GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS},
        {"MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS", GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS},
        {"MAX_COMBINED_TEXTURE_IMAGE_UNITS", GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS},
        {"MAX_COMBINED_UNIFORM_BLOCKS", GL_MAX_COMBINED_UNIFORM_BLOCKS},
        {"MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS", GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS},
        {"MAX_CUBE_MAP_TEXTURE_SIZE", GL_MAX_CUBE_MAP_TEXTURE_SIZE},
        {"MAX_DEPTH_TEXTURE_SAMPLES", GL_MAX_DEPTH_TEXTURE_SAMPLES},
        {"MAX_DRAW_BUFFERS", GL_MAX_DRAW_BUFFERS},
        {"MAX_DUAL_SOURCE_DRAW_BUFFERS", GL_MAX_DUAL_SOURCE_DRAW_BUFFERS},
        {"MAX_ELEMENTS_INDICES", GL_MAX_ELEMENTS_INDICES},
        {"MAX_FRAGMENT_ATOMIC_COUNTERS", GL_MAX_FRAGMENT_ATOMIC_COUNTERS},
        {"MAX_FRAGMENT_SHADER_STORAGE_BLOCKS", GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS},
        {"MAX_FRAGMENT_INPUT_COMPONENTS", GL_MAX_FRAGMENT_INPUT_COMPONENTS},
        {"MAX_FRAGMENT_UNIFORM_COMPONENTS", GL_MAX_FRAGMENT_UNIFORM_COMPONENTS},
        {"MAX_FRAGMENT_UNIFORM_VECTORS", GL_MAX_FRAGMENT_UNIFORM_VECTORS},
        {"MAX_FRAGMENT_UNIFORM_BLOCKS", GL_MAX_FRAGMENT_UNIFORM_BLOCKS},
        {"MAX_FRAMEBUFFER_WIDTH", GL_MAX_FRAMEBUFFER_WIDTH},
        {"MAX_FRAMEBUFFER_HEIGHT", GL_MAX_FRAMEBUFFER_HEIGHT},
        {"MAX_FRAMEBUFFER_LAYERS", GL_MAX_FRAMEBUFFER_LAYERS},
        {"MAX_FRAMEBUFFER_SAMPLES", GL_MAX_FRAMEBUFFER_SAMPLES},
        {"MAX_GEOMETRY_ATOMIC_COUNTERS", GL_MAX_GEOMETRY_ATOMIC_COUNTERS},
        {"MAX_GEOMETRY_SHADER_STORAGE_BLOCKS", GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS},
        {"MAX_GEOMETRY_INPUT_COMPONENTS", GL_MAX_GEOMETRY_INPUT_COMPONENTS},
        {"MAX_GEOMETRY_OUTPUT_COMPONENTS", GL_MAX_GEOMETRY_OUTPUT_COMPONENTS},
        {"MAX_GEOMETRY_TEXTURE_IMAGE_UNITS", GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS},
        {"MAX_GEOMETRY_UNIFORM_BLOCKS", GL_MAX_GEOMETRY_UNIFORM_BLOCKS},
        {"MAX_GEOMETRY_UNIFORM_COMPONENTS", GL_MAX_GEOMETRY_UNIFORM_COMPONENTS},
        {"MAX_INTEGER_SAMPLES", GL_MAX_INTEGER_SAMPLES},
        {"MIN_MAP_BUFFER_ALIGNMENT", GL_MIN_MAP_BUFFER_ALIGNMENT},
        {"MAX_LABEL_LENGTH", GL_MAX_LABEL_LENGTH},
        {"MAX_PROGRAM_TEXEL_OFFSET", GL_MAX_PROGRAM_TEXEL_OFFSET},
        {"MIN_PROGRAM_TEXEL_OFFSET", GL_MIN_PROGRAM_TEXEL_OFFSET},
        {"MAX_RECTANGLE_TEXTURE_SIZE", GL_MAX_RECTANGLE_TEXTURE_SIZE},
        {"MAX_RENDERBUFFER_SIZE", GL_MAX_RENDERBUFFER_SIZE},
        {"MAX_SAMPLE_MASK_WORDS", GL_MAX_SAMPLE_MASK_WORDS},
        {"MAX_SERVER_WAIT_TIMEOUT", GL_MAX_SERVER_WAIT_TIMEOUT},
        {"MAX_SHADER_STORAGE_BUFFER_BINDINGS", GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS},
        {"MAX_TESS_CONTROL_ATOMIC_COUNTERS", GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS},
        {"MAX_TESS_EVALUATION_ATOMIC_COUNTERS", GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS},
        {"MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS", GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS},
        {"MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS", GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS},
        {"MAX_TEXTURE_BUFFER_SIZE", GL_MAX_TEXTURE_BUFFER_SIZE},
        {"MAX_TEXTURE_IMAGE_UNITS", GL_MAX_TEXTURE_IMAGE_UNITS},
        {"MAX_TEXTURE_LOD_BIAS", GL_MAX_TEXTURE_LOD_BIAS},
        {"MAX_TEXTURE_SIZE", GL_MAX_TEXTURE_SIZE},
        {"MAX_UNIFORM_BUFFER_BINDINGS", GL_MAX_UNIFORM_BUFFER_BINDINGS},
        {"MAX_UNIFORM_BLOCK_SIZE", GL_MAX_UNIFORM_BLOCK_SIZE},
        {"MAX_UNIFORM_LOCATIONS", GL_MAX_UNIFORM_LOCATIONS},
        {"MAX_VARYING_COMPONENTS", GL_MAX_VARYING_COMPONENTS},
        {"MAX_VARYING_VECTORS", GL_MAX_VARYING_VECTORS},
        {"MAX_VARYING_FLOATS", GL_MAX_VARYING_FLOATS},
        {"MAX_VERTEX_ATOMIC_COUNTERS", GL_MAX_VERTEX_ATOMIC_COUNTERS},
        {"MAX_VERTEX_ATTRIBS", GL_MAX_VERTEX_ATTRIBS},
        {"MAX_VERTEX_SHADER_STORAGE_BLOCKS", GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS},
        {"MAX_VERTEX_TEXTURE_IMAGE_UNITS", GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS},
        {"MAX_VERTEX_UNIFORM_COMPONENTS", GL_MAX_VERTEX_UNIFORM_COMPONENTS},
        {"MAX_VERTEX_UNIFORM_VECTORS", GL_MAX_VERTEX_UNIFORM_VECTORS},
        {"MAX_VERTEX_OUTPUT_COMPONENTS", GL_MAX_VERTEX_OUTPUT_COMPONENTS},
        {"MAX_VERTEX_UNIFORM_BLOCKS", GL_MAX_VERTEX_UNIFORM_BLOCKS},
        {"MAX_VIEWPORT_DIMS", GL_MAX_VIEWPORT_DIMS},
        {"MAX_VIEWPORTS", GL_MAX_VIEWPORTS},
        {"MINOR_VERSION", GL_MINOR_VERSION},
        {"NUM_COMPRESSED_TEXTURE_FORMATS", GL_NUM_COMPRESSED_TEXTURE_FORMATS},
        {"NUM_EXTENSIONS", GL_NUM_EXTENSIONS},
        {"NUM_PROGRAM_BINARY_FORMATS", GL_NUM_PROGRAM_BINARY_FORMATS},
        {"NUM_SHADER_BINARY_FORMATS", GL_NUM_SHADER_BINARY_FORMATS},
        {"PACK_ALIGNMENT", GL_PACK_ALIGNMENT},
        {"PACK_IMAGE_HEIGHT", GL_PACK_IMAGE_HEIGHT},
        {"PACK_LSB_FIRST", GL_PACK_LSB_FIRST},
        {"PACK_ROW_LENGTH", GL_PACK_ROW_LENGTH},
        {"PACK_SKIP_IMAGES", GL_PACK_SKIP_IMAGES},
        {"PACK_SKIP_PIXELS", GL_PACK_SKIP_PIXELS},
        {"PACK_SKIP_ROWS", GL_PACK_SKIP_ROWS},
        {"PACK_SWAP_BYTES", GL_PACK_SWAP_BYTES},
        {"PIXEL_PACK_BUFFER_BINDING", GL_PIXEL_PACK_BUFFER_BINDING},
        {"PIXEL_UNPACK_BUFFER_BINDING", GL_PIXEL_UNPACK_BUFFER_BINDING},
        {"POINT_FADE_THRESHOLD_SIZE", GL_POINT_FADE_THRESHOLD_SIZE},
        {"PRIMITIVE_RESTART_INDEX", GL_PRIMITIVE_RESTART_INDEX},
        {"PROGRAM_BINARY_FORMATS", GL_PROGRAM_BINARY_FORMATS},
        {"PROGRAM_PIPELINE_BINDING", GL_PROGRAM_PIPELINE_BINDING},
        {"PROGRAM_POINT_SIZE", GL_PROGRAM_POINT_SIZE},
        {"PROVOKING_VERTEX", GL_PROVOKING_VERTEX},
        {"POINT_SIZE", GL_POINT_SIZE},
        {"POINT_SIZE_GRANULARITY", GL_POINT_SIZE_GRANULARITY},
        {"POINT_SIZE_RANGE", GL_POINT_SIZE_RANGE},
        {"POLYGON_OFFSET_FACTOR", GL_POLYGON_OFFSET_FACTOR},
        {"POLYGON_OFFSET_UNITS", GL_POLYGON_OFFSET_UNITS},
        {"POLYGON_OFFSET_FILL", GL_POLYGON_OFFSET_FILL},
        {"POLYGON_OFFSET_LINE", GL_POLYGON_OFFSET_LINE},
        {"POLYGON_OFFSET_POINT", GL_POLYGON_OFFSET_POINT},
        {"POLYGON_SMOOTH", GL_POLYGON_SMOOTH},
        {"POLYGON_SMOOTH_HINT", GL_POLYGON_SMOOTH_HINT},
        {"READ_BUFFER", GL_READ_BUFFER},
        {"RENDERBUFFER_BINDING", GL_RENDERBUFFER_BINDING},
        {"SAMPLE_BUFFERS", GL_SAMPLE_BUFFERS},
        {"SAMPLE_COVERAGE_VALUE", GL_SAMPLE_COVERAGE_VALUE},
        {"SAMPLE_COVERAGE_INVERT", GL_SAMPLE_COVERAGE_INVERT},
        {"SAMPLE_MASK_VALUE", GL_SAMPLE_MASK_VALUE},
        {"SAMPLER_BINDING", GL_SAMPLER_BINDING},
        {"SAMPLES", GL_SAMPLES},
        {"SCISSOR_BOX", GL_SCISSOR_BOX},
        {"SCISSOR_TEST", GL_SCISSOR_TEST},
        {"SHADER_COMPILER", GL_SHADER_COMPILER},
        {"SHADER_STORAGE_BUFFER_BINDING", GL_SHADER_STORAGE_BUFFER_BINDING},
        {"SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT", GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT},
        {"SHADER_STORAGE_BUFFER_START", GL_SHADER_STORAGE_BUFFER_START},
        {"SHADER_STORAGE_BUFFER_SIZE", GL_SHADER_STORAGE_BUFFER_SIZE},
        {"SMOOTH_LINE_WIDTH_RANGE", GL_SMOOTH_LINE_WIDTH_RANGE},
        {"SMOOTH_LINE_WIDTH_GRANULARITY", GL_SMOOTH_LINE_WIDTH_GRANULARITY},
        {"STENCIL_BACK_FAIL", GL_STENCIL_BACK_FAIL},
        {"STENCIL_BACK_FUNC", GL_STENCIL_BACK_FUNC},
        {"STENCIL_BACK_PASS_DEPTH_FAIL", GL_STENCIL_BACK_PASS_DEPTH_FAIL},
        {"STENCIL_BACK_PASS_DEPTH_PASS", GL_STENCIL_BACK_PASS_DEPTH_PASS},
        {"STENCIL_BACK_REF", GL_STENCIL_BACK_REF},
        {"STENCIL_BACK_VALUE_MASK", GL_STENCIL_BACK_VALUE_MASK},
        {"STENCIL_BACK_WRITEMASK", GL_STENCIL_BACK_WRITEMASK},
        {"STENCIL_CLEAR_VALUE", GL_STENCIL_CLEAR_VALUE},
        {"STENCIL_FAIL", GL_STENCIL_FAIL},
        {"STENCIL_FUNC", GL_STENCIL_FUNC},
        {"STENCIL_PASS_DEPTH_FAIL", GL_STENCIL_PASS_DEPTH_FAIL},
        {"STENCIL_PASS_DEPTH_PASS", GL_STENCIL_PASS_DEPTH_PASS},
        {"STENCIL_REF", GL_STENCIL_REF},
        {"STENCIL_TEST", GL_STENCIL_TEST},
        {"STENCIL_VALUE_MASK", GL_STENCIL_VALUE_MASK},
        {"STENCIL_WRITEMASK", GL_STENCIL_WRITEMASK},
        {"STEREO", GL_STEREO},
        {"SUBPIXEL_BITS", GL_SUBPIXEL_BITS},
        {"TEXTURE_BINDING_1D", GL_TEXTURE_BINDING_1D},
        {"TEXTURE_BINDING_1D_ARRAY", GL_TEXTURE_BINDING_1D_ARRAY},
        {"TEXTURE_BINDING_2D", GL_TEXTURE_BINDING_2D},
        {"TEXTURE_BINDING_2D_ARRAY", GL_TEXTURE_BINDING_2D_ARRAY},
        {"TEXTURE_BINDING_2D_MULTISAMPLE", GL_TEXTURE_BINDING_2D_MULTISAMPLE},
        {"TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY", GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY},
        {"TEXTURE_BINDING_3D", GL_TEXTURE_BINDING_3D},
        {"TEXTURE_BINDING_BUFFER", GL_TEXTURE_BINDING_BUFFER},
        {"TEXTURE_BINDING_CUBE_MAP", GL_TEXTURE_BINDING_CUBE_MAP},
        {"TEXTURE_BINDING_RECTANGLE", GL_TEXTURE_BINDING_RECTANGLE},
        {"TEXTURE_COMPRESSION_HINT", GL_TEXTURE_COMPRESSION_HINT},
        {"TEXTURE_BUFFER_OFFSET_ALIGNMENT", GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT},
        {"TIMESTAMP", GL_TIMESTAMP},
        {"TRANSFORM_FEEDBACK_BUFFER_BINDING", GL_TRANSFORM_FEEDBACK_BUFFER_BINDING},
        {"TRANSFORM_FEEDBACK_BUFFER_START", GL_TRANSFORM_FEEDBACK_BUFFER_START},
        {"TRANSFORM_FEEDBACK_BUFFER_SIZE", GL_TRANSFORM_FEEDBACK_BUFFER_SIZE},
        {"UNIFORM_BUFFER_BINDING", GL_UNIFORM_BUFFER_BINDING},
        {"UNIFORM_BUFFER_OFFSET_ALIGNMENT", GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT},
        {"UNIFORM_BUFFER_SIZE", GL_UNIFORM_BUFFER_SIZE},
        {"UNIFORM_BUFFER_START", GL_UNIFORM_BUFFER_START},
        {"UNPACK_ALIGNMENT", GL_UNPACK_ALIGNMENT},
        {"UNPACK_IMAGE_HEIGHT", GL_UNPACK_IMAGE_HEIGHT},
        {"UNPACK_LSB_FIRST", GL_UNPACK_LSB_FIRST},
        {"UNPACK_ROW_LENGTH", GL_UNPACK_ROW_LENGTH},
        {"UNPACK_SKIP_IMAGES", GL_UNPACK_SKIP_IMAGES},
        {"UNPACK_SKIP_PIXELS", GL_UNPACK_SKIP_PIXELS},
        {"UNPACK_SKIP_ROWS", GL_UNPACK_SKIP_ROWS},
        {"UNPACK_SWAP_BYTES", GL_UNPACK_SWAP_BYTES},
        {"VERTEX_ARRAY_BINDING", GL_VERTEX_ARRAY_BINDING},
        {"VERTEX_BINDING_DIVISOR", GL_VERTEX_BINDING_DIVISOR},
        {"VERTEX_BINDING_OFFSET", GL_VERTEX_BINDING_OFFSET},
        {"VERTEX_BINDING_STRIDE", GL_VERTEX_BINDING_STRIDE},
        {"VERTEX_BINDING_BUFFER", GL_VERTEX_BINDING_BUFFER},
        {"MAX_VERTEX_ATTRIB_RELATIVE_OFFSET", GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET},
        {"MAX_VERTEX_ATTRIB_BINDINGS", GL_MAX_VERTEX_ATTRIB_BINDINGS},
        {"VIEWPORT", GL_VIEWPORT},
        {"VIEWPORT_BOUNDS_RANGE", GL_VIEWPORT_BOUNDS_RANGE},
        {"VIEWPORT_INDEX_PROVOKING_VERTEX", GL_VIEWPORT_INDEX_PROVOKING_VERTEX},
        {"VIEWPORT_SUBPIXEL_BITS", GL_VIEWPORT_SUBPIXEL_BITS},
        {"MAX_ELEMENT_INDEX", GL_MAX_ELEMENT_INDEX},
        {0}}};

static ModuleInfo modInfo = {
    .def = {
        PyModuleDef_HEAD_INIT,
        .m_name = "pygl.context",
        .m_size = -1,
        .m_doc = PyDoc_STR("A module that provides functions used to retrieve info about current OpenGL context."),
        .m_methods = (PyMethodDef[]){
            {"get_integer", (PyCFunction)py_get_integer, METH_O, PyDoc_STR("Wrapper for glGetIntegerv (see: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml).")},
            {"is_enabled", (PyCFunction)py_is_enabled, METH_O, PyDoc_STR("Wrapper for glIsEnabled (see: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glIsEnabled.xhtml).")},
            {0}}},
    .enums = (EnumDef *[]){&getNameEnum, {0}}};

PyMODINIT_FUNC PyInit_context()
{
    return module_create_from_info(&modInfo);
}