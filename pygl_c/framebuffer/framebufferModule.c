#include "renderbufferAttachment.h"
#include "textureAttachment.h"
#include "framebuffer.h"
#include "../module.h"

static EnumDef attachmentFormatEnum = {
    .enumName = "AttachmentFormat",
    .values = (EnumValue[]){
        {"RGBA8", GL_RGBA8},
        {"RGB8", GL_RGB8},
        {"RG8", GL_RG8},
        {"R8", GL_R8},
        {"RGBA16", GL_RGBA16},
        {"RGB16", GL_RGB16},
        {"RG16", GL_RG16},
        {"R16", GL_R16},
        {"RGBA16F", GL_RGBA16F},
        {"RGB16F", GL_RGB16F},
        {"RG16F", GL_RG16F},
        {"R16F", GL_R16F},
        {"RGBA32F", GL_RGBA32F},
        {"RGB32F", GL_RGB32F},
        {"RG32F", GL_RG32F},
        {"R32F", GL_R32F},
        {"RGBA8I", GL_RGBA8I},
        {"RGB8I", GL_RGB8I},
        {"RG8I", GL_RG8I},
        {"R8I", GL_R8I},
        {"RGBA16I", GL_RGBA16I},
        {"RGB16I", GL_RGB16I},
        {"RG16I", GL_RG16I},
        {"R16I", GL_R16I},
        {"RGBA32I", GL_RGBA32I},
        {"RGB32I", GL_RGB32I},
        {"RG32I", GL_RG32I},
        {"R32I", GL_R32I},
        {"RGBA8UI", GL_RGBA8UI},
        {"RGB8UI", GL_RGB8UI},
        {"RG8UI", GL_RG8UI},
        {"R8UI", GL_R8UI},
        {"RGBA16UI", GL_RGBA16UI},
        {"RGB16UI", GL_RGB16UI},
        {"RG16UI", GL_RG16UI},
        {"R16UI", GL_R16UI},
        {"RGBA32UI", GL_RGBA32UI},
        {"RGB32UI", GL_RGB32UI},
        {"RG32UI", GL_RG32UI},
        {"R32UI", GL_R32UI},
        {"RGBA4", GL_RGBA4},
        {"RGB5_A1", GL_RGB5_A1},
        {"RGB565", GL_RGB565},
        {"RGB10_A2", GL_RGB10_A2},
        {"RGB10_A2UI", GL_RGB10_A2UI},
        {"R11F_G11F_B10F", GL_R11F_G11F_B10F},
        {"SRGB8_ALPHA8", GL_SRGB8_ALPHA8},
        {"DEPTH_COMPONENT16", GL_DEPTH_COMPONENT16},
        {"DEPTH_COMPONENT24", GL_DEPTH_COMPONENT24},
        {"DEPTH_COMPONENT32F", GL_DEPTH_COMPONENT32F},
        {"DEPTH24_STENCIL8", GL_DEPTH24_STENCIL8},
        {"DEPTH32F_STENCIL8", GL_DEPTH32F_STENCIL8},
        {"STENCIL_INDEX8", GL_STENCIL_INDEX8},
        {0}},
};

static EnumDef attachmentEnum = {
    .enumName = "Attachment",
    .values = (EnumValue[]){
        {"DEPTH_ATTACHMENT", GL_DEPTH_ATTACHMENT},
        {"STENCIL_ATTACHMENT", GL_STENCIL_ATTACHMENT},
        {"DEPTH_STENCIL_ATTACHMENT", GL_DEPTH_STENCIL_ATTACHMENT},
        {0}},
};

static ModuleInfo modInfo = {
    .def = {
        PyModuleDef_HEAD_INIT,
        .m_name = "pygl.framebuffer",
        .m_size = -1},
    .enums = (EnumDef *[]){&attachmentEnum, &attachmentFormatEnum, NULL},
    .types = (PyTypeObject *[]){&pyTextureAttachmentType, &pyRenderbufferAttachmentType, &pyFramebufferType, NULL},
};

PyMODINIT_FUNC PyInit_framebuffer()
{
    return module_create_from_info(&modInfo);
}
