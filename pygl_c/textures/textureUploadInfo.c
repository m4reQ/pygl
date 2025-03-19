#include "texture.h"

static PyObject *is_compressed_get(PyTextureUploadInfo *self, void *Py_UNUSED(closure))
{
    return PyBool_FromLong(self->imageSize > 0);
}

static int init(PyTextureUploadInfo *self, PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {
        "format",
        "width",
        "height",
        /* optional */
        "depth",           // = 1
        "x_offset",        // = 0
        "y_offset",        // = 0
        "z_offset",        // = 0
        "level",           // = 0
        "alignment",       // = 4
        "pixel_type",      // = GL_UNSIGNED_BYTE
        "image_size",      // = 0
        "data_offset",     // = 0
        "generate_mipmap", // = True
        NULL,
    };

    self->depth = 1;
    self->alignment = 4;
    self->pixelType = GL_UNSIGNED_BYTE;
    self->generateMipmap = true;

    if (!PyArg_ParseTupleAndKeywords(
            args, kwargs, "Iii|iiiiiiIinp", kwNames,
            &self->format,
            &self->width, &self->height, &self->depth,
            &self->xOffset, &self->yOffset, &self->zOffset,
            &self->level,
            &self->alignment,
            &self->pixelType,
            &self->imageSize, &self->dataOffset,
            &self->generateMipmap))
        return -1;

    return 0;
}

PyTypeObject pyTextureUploadInfoType = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_name = "pygl.textures.TextureUploadInfo",
    .tp_basicsize = sizeof(PyTextureUploadInfo),
    .tp_init = (initproc)init,
    .tp_members = (PyMemberDef[]){
        {"width", Py_T_INT, offsetof(PyTextureUploadInfo, width), 0, NULL},
        {"height", Py_T_INT, offsetof(PyTextureUploadInfo, height), 0, NULL},
        {"depth", Py_T_INT, offsetof(PyTextureUploadInfo, depth), 0, NULL},
        {"x_offset", Py_T_INT, offsetof(PyTextureUploadInfo, xOffset), 0, NULL},
        {"y_offset", Py_T_INT, offsetof(PyTextureUploadInfo, yOffset), 0, NULL},
        {"z_offset", Py_T_INT, offsetof(PyTextureUploadInfo, zOffset), 0, NULL},
        {"level", Py_T_INT, offsetof(PyTextureUploadInfo, level), 0, NULL},
        {"format", Py_T_UINT, offsetof(PyTextureUploadInfo, format), 0, NULL},
        {"pixel_type", Py_T_UINT, offsetof(PyTextureUploadInfo, pixelType), 0, NULL},
        {"image_size", Py_T_INT, offsetof(PyTextureUploadInfo, imageSize), 0, NULL},
        {"data_offset", Py_T_PYSSIZET, offsetof(PyTextureUploadInfo, dataOffset), 0, NULL},
        {"generate_mipmap", Py_T_BOOL, offsetof(PyTextureUploadInfo, generateMipmap), 0, NULL},
        {"alignment", Py_T_INT, offsetof(PyTextureUploadInfo, alignment), 0, NULL},
        {0},
    },
    .tp_getset = (PyGetSetDef[]){
        {"is_compressed", (getter)is_compressed_get, NULL, NULL, NULL},
        {0},
    },
};
