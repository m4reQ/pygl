#include "textureSpec.h"
#include "texture2D.h"
#include "texture2DArray.h"
#include "uploadInfo.h"
#include "textureBase.h"
#include "../module.h"
#include "../utility.h"
#include <glad/gl.h>

static PyObject *_set_pixel_storei(GLenum param, PyObject *value)
{
    THROW_IF(
        !PyLong_Check(value),
        PyExc_TypeError,
        "argument has to be of type int.",
        NULL);

    glPixelStorei(param, PyLong_AsLong(value));

    Py_RETURN_NONE;
}

static PyObject *set_pixel_pack_alignment(PyObject *Py_UNUSED(self), PyObject *alignment)
{
    return _set_pixel_storei(GL_PACK_ALIGNMENT, alignment);
}

static PyObject *set_pixel_unpack_alignment(PyObject *Py_UNUSED(self), PyObject *alignment)
{
    return _set_pixel_storei(GL_PACK_ALIGNMENT, alignment);
}

static PyObject *bind_textures(PyObject *Py_UNUSED(cls), PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {"textures", "first", NULL};

    GLuint first = 0;
    PyObject *textures = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!|I", kwNames, &PyList_Type, &textures, &first))
        return NULL;

    GLsizei nTextures = PyList_GET_SIZE(textures);
    if (nTextures == 0)
        Py_RETURN_NONE;

    GLuint *textureIds = PyMem_Malloc(sizeof(GLuint) * nTextures);
    for (GLsizei i = 0; i < nTextures; i++)
    {
        PyTexture *tex = (PyTexture *)PyList_GET_ITEM(textures, i);
        textureIds[i] = tex->id;
    }

    glBindTextures(first, nTextures, textureIds);

    PyMem_Free(textureIds);
    Py_RETURN_NONE;
}

static EnumDef magFilterEnum = {
    .enumName = "MagFilter",
    .values = (EnumValue[]){
        {"NEAREST", GL_NEAREST},
        {"LINEAR", GL_LINEAR},
        {0}},
};

static EnumDef minFilterEnum = {
    .enumName = "MinFilter",
    .values = (EnumValue[]){
        {"NEAREST", GL_NEAREST},
        {"LINEAR", GL_LINEAR},
        {"NEAREST_MIPMAP_NEAREST", GL_NEAREST_MIPMAP_NEAREST},
        {"LINEAR_MIPMAP_NEAREST", GL_LINEAR_MIPMAP_NEAREST},
        {"NEAREST_MIPMAP_LINEAR", GL_NEAREST_MIPMAP_LINEAR},
        {"LINEAR_MIPMAP_LINEAR", GL_LINEAR_MIPMAP_LINEAR},
        {0}},
};

static EnumDef textureParameterEnum = {
    .enumName = "TextureParameter",
    .values = (EnumValue[]){
        {"DEPTH_STENCIL_TEXTURE_MODE", GL_DEPTH_STENCIL_TEXTURE_MODE},
        {"TEXTURE_BASE_LEVEL", GL_TEXTURE_BASE_LEVEL},
        {"TEXTURE_BORDER_COLOR", GL_TEXTURE_BORDER_COLOR},
        {"TEXTURE_COMPARE_FUNC", GL_TEXTURE_COMPARE_FUNC},
        {"TEXTURE_COMPARE_MODE", GL_TEXTURE_COMPARE_MODE},
        {"TEXTURE_LOD_BIAS", GL_TEXTURE_LOD_BIAS},
        {"TEXTURE_MIN_FILTER", GL_TEXTURE_MIN_FILTER},
        {"TEXTURE_MAG_FILTER", GL_TEXTURE_MAG_FILTER},
        {"TEXTURE_MIN_LOD", GL_TEXTURE_MIN_LOD},
        {"TEXTURE_MAX_LOD", GL_TEXTURE_MAX_LOD},
        {"TEXTURE_MAX_LEVEL", GL_TEXTURE_MAX_LEVEL},
        {"TEXTURE_SWIZZLE_R", GL_TEXTURE_SWIZZLE_R},
        {"TEXTURE_SWIZZLE_G", GL_TEXTURE_SWIZZLE_G},
        {"TEXTURE_SWIZZLE_B", GL_TEXTURE_SWIZZLE_B},
        {"TEXTURE_SWIZZLE_A", GL_TEXTURE_SWIZZLE_A},
        {"TEXTURE_SWIZZLE_RGBA", GL_TEXTURE_SWIZZLE_RGBA},
        {"TEXTURE_WRAP_S", GL_TEXTURE_WRAP_S},
        {"TEXTURE_WRAP_T", GL_TEXTURE_WRAP_T},
        {"TEXTURE_WRAP_R", GL_TEXTURE_WRAP_R},
        {0}},
};

static EnumDef wrapModeEnum = {
    .enumName = "WrapMode",
    .values = (EnumValue[]){
        {"CLAMP_TO_EDGE", GL_CLAMP_TO_EDGE},
        {"CLAMP_TO_BORDER", GL_CLAMP_TO_BORDER},
        {"MIRROR_CLAMP_TO_EDGE", GL_MIRROR_CLAMP_TO_EDGE},
        {"REPEAT", GL_REPEAT},
        {"MIRRORED_REPEAT", GL_MIRRORED_REPEAT},
        {0}},
};

static EnumDef internalFormatEnum = {
    .enumName = "InternalFormat",
    .values = (EnumValue[]){
        {"R8", GL_R8},
        {"R8_SNORM", GL_R8_SNORM},
        {"R16", GL_R16},
        {"R16_SNORM", GL_R16_SNORM},
        {"RG8", GL_RG8},
        {"RG8_SNORM", GL_RG8_SNORM},
        {"RG16", GL_RG16},
        {"RG16_SNORM", GL_RG16_SNORM},
        {"R3_G3_B2", GL_R3_G3_B2},
        {"RGB4", GL_RGB4},
        {"RGB5", GL_RGB5},
        {"RGB8", GL_RGB8},
        {"RGB8_SNORM", GL_RGB8_SNORM},
        {"RGB10", GL_RGB10},
        {"RGB12", GL_RGB12},
        {"RGB16_SNORM", GL_RGB16_SNORM},
        {"RGBA2", GL_RGBA2},
        {"RGBA4", GL_RGBA4},
        {"RGB5_A1", GL_RGB5_A1},
        {"RGBA8", GL_RGBA8},
        {"RGBA8_SNORM", GL_RGBA8_SNORM},
        {"RGB10_A2", GL_RGB10_A2},
        {"RGB10_A2UI", GL_RGB10_A2UI},
        {"RGBA12", GL_RGBA12},
        {"RGBA16", GL_RGBA16},
        {"SRGB8", GL_SRGB8},
        {"SRGB8_ALPHA8", GL_SRGB8_ALPHA8},
        {"R16F", GL_R16F},
        {"RG16F", GL_RG16F},
        {"RGB16F", GL_RGB16F},
        {"RGBA16F", GL_RGBA16F},
        {"R32F", GL_R32F},
        {"RG32F", GL_RG32F},
        {"RGB32F", GL_RGB32F},
        {"RGBA32F", GL_RGBA32F},
        {"R11F_G11F_B10F", GL_R11F_G11F_B10F},
        {"RGB9_E5", GL_RGB9_E5},
        {"R8I", GL_R8I},
        {"R8UI", GL_R8UI},
        {"R16I", GL_R16I},
        {"R16UI", GL_R16UI},
        {"R32I", GL_R32I},
        {"R32UI", GL_R32UI},
        {"RG8I", GL_RG8I},
        {"RG8UI", GL_RG8UI},
        {"RG16I", GL_RG16I},
        {"RG16UI", GL_RG16UI},
        {"RG32I", GL_RG32I},
        {"RG32UI", GL_RG32UI},
        {"RGB8I", GL_RGB8I},
        {"RGB8UI", GL_RGB8UI},
        {"RGB16I", GL_RGB16I},
        {"RGB16UI", GL_RGB16UI},
        {"RGB32I", GL_RGB32I},
        {"RGB32UI", GL_RGB32UI},
        {"RGBA8I", GL_RGBA8I},
        {"RGBA8UI", GL_RGBA8UI},
        {"RGBA16I", GL_RGBA16I},
        {"RGBA16UI", GL_RGBA16UI},
        {"RGBA32I", GL_RGBA32I},
        {"RGBA32UI", GL_RGBA32UI},
        {0}},
};

static EnumDef pixelFormatEnum = {
    .enumName = "PixelFormat",
    .values = (EnumValue[]){
        {"RED", GL_RED},
        {"RG", GL_RG},
        {"RGB", GL_RGB},
        {"BGR", GL_BGR},
        {"RGBA", GL_RGBA},
        {"BGRA", GL_BGRA},
        {"DEPTH_COMPONENT", GL_DEPTH_COMPONENT},
        {"STENCIL_INDEX", GL_STENCIL_INDEX},
        {0}},
};

static EnumDef pixelTypeEnum = {
    .enumName = "PixelType",
    .values = (EnumValue[]){
        {"UNSIGNED_BYTE", GL_UNSIGNED_BYTE},
        {"BYTE", GL_BYTE},
        {"UNSIGNED_SHORT", GL_UNSIGNED_SHORT},
        {"SHORT", GL_SHORT},
        {"UNSIGNED_INT", GL_UNSIGNED_INT},
        {"INT", GL_INT},
        {"FLOAT", GL_FLOAT},
        {"UNSIGNED_BYTE_3_3_2", GL_UNSIGNED_BYTE_3_3_2},
        {"UNSIGNED_BYTE_2_3_3_REV", GL_UNSIGNED_BYTE_2_3_3_REV},
        {"UNSIGNED_SHORT_5_6_5", GL_UNSIGNED_SHORT_5_6_5},
        {"UNSIGNED_SHORT_5_6_5_REV", GL_UNSIGNED_SHORT_5_6_5_REV},
        {"UNSIGNED_SHORT_4_4_4_4", GL_UNSIGNED_SHORT_4_4_4_4},
        {"UNSIGNED_SHORT_4_4_4_4_REV", GL_UNSIGNED_SHORT_4_4_4_4_REV},
        {"UNSIGNED_SHORT_5_5_5_1", GL_UNSIGNED_SHORT_5_5_5_1},
        {"UNSIGNED_SHORT_1_5_5_5_REV", GL_UNSIGNED_SHORT_1_5_5_5_REV},
        {"UNSIGNED_INT_8_8_8_8", GL_UNSIGNED_INT_8_8_8_8},
        {"UNSIGNED_INT_8_8_8_8_REV", GL_UNSIGNED_INT_8_8_8_8_REV},
        {"UNSIGNED_INT_10_10_10_2", GL_UNSIGNED_INT_10_10_10_2},
        {"UNSIGNED_INT_2_10_10_10_REV", GL_UNSIGNED_INT_2_10_10_10_REV},
        {0}},
};

static EnumDef compressedInternalFormatEnum = {
    .enumName = "CompressedInternalFormat",
    .values = (EnumValue[]){
        {"COMPRESSED_RGB_S3TC_DXT1_EXT", GL_COMPRESSED_RGB_S3TC_DXT1_EXT},
        {"COMPRESSED_RGBA_S3TC_DXT1_EXT", GL_COMPRESSED_RGBA_S3TC_DXT1_EXT},
        {"COMPRESSED_RGBA_S3TC_DXT3_EXT", GL_COMPRESSED_RGBA_S3TC_DXT3_EXT},
        {"COMPRESSED_RGBA_S3TC_DXT5_EXT", GL_COMPRESSED_RGBA_S3TC_DXT5_EXT},
        {0}},
};

static ModuleInfo modInfo = {
    .def = {
        PyModuleDef_HEAD_INIT,
        .m_name = "pygl.textures",
        .m_size = -1,
        .m_methods = (PyMethodDef[]){
            {"bind_textures", (PyCFunction)bind_textures, METH_VARARGS | METH_KEYWORDS, NULL},
            {"set_pixel_pack_alignment", (PyCFunction)set_pixel_pack_alignment, METH_O, NULL},
            {"set_pixel_unpack_alignment", (PyCFunction)set_pixel_unpack_alignment, METH_O, NULL},
            {0}}},
    .enums = (EnumDef *[]){&textureParameterEnum, &minFilterEnum, &magFilterEnum, &wrapModeEnum, &internalFormatEnum, &pixelFormatEnum, &pixelTypeEnum, &compressedInternalFormatEnum, NULL},
    .types = (PyTypeObject *[]){&pyTextureSpecType, &pyTexture2DType, &pyTexture2DArrayType, &pyUploadInfoType, NULL},
};

PyMODINIT_FUNC PyInit_textures()
{
    return module_create_from_info(&modInfo);
}
