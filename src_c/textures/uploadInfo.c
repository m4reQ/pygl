#include "uploadInfo.h"
#include <structmember.h>

static PyObject* py_upload_info_is_compressed_get(PyUploadInfo* self, void* Py_UNUSED(closure))
{
    return PyBool_FromLong(self->imageSize > 0);
}

static int py_upload_info_init(PyUploadInfo* self, PyObject* args, PyObject* kwargs)
{
    static char* kwNames[] = {
        "width", "height", "format",
        /* optional */
        "pixel_type", "level", "x_offset", "y_offset", "image_size", "generate_mipmap",
        NULL};

    // TODO find out if objects are zero-initialized, if so remove assignments with 0
    self->type = GL_UNSIGNED_BYTE;
    self->level = 0;
    self->xOffset = 0;
    self->yOffset = 0;
    self->imageSize = 0;
    if (!PyArg_ParseTupleAndKeywords(
        args, kwargs, "iiI|Iiiiip", kwNames,
        &self->width, &self->height, &self->format,
        &self->type, &self->level, &self->xOffset, &self->yOffset,
        &self->imageSize, &self->generateMipmap))
        return -1;

    if (self->width < 0 || self->height < 0)
    {
        PyErr_SetString(PyExc_ValueError, "Both width and height must be greater than 0.");
        return -1;
    }

    if (self->imageSize < 0)
    {
        PyErr_SetString(PyExc_ValueError, "Image size for compressed texture must be greater than 0.");
        return -1;
    }

    return 0;
}

static PyMemberDef pyUploadInfoMembers[] = {
    {"width", T_INT, offsetof(PyUploadInfo, width), 0, NULL},
    {"height", T_INT, offsetof(PyUploadInfo, height), 0, NULL},
    {"format", T_UINT, offsetof(PyUploadInfo, format), 0, NULL},
    {"pixel_type", T_UINT, offsetof(PyUploadInfo, type), 0, NULL},
    {"level", T_INT, offsetof(PyUploadInfo, level), 0, NULL},
    {"x_offset", T_INT, offsetof(PyUploadInfo, xOffset), 0, NULL},
    {"y_offset", T_INT, offsetof(PyUploadInfo, yOffset), 0, NULL},
    {"image_size", T_INT, offsetof(PyUploadInfo, imageSize), 0, NULL},
    {"generate_mipmap", T_BOOL, offsetof(PyUploadInfo, generateMipmap), 0, NULL},
    {0},
};

static PyGetSetDef pyUploadInfoGetSet[] = {
    {"is_compressed", (getter)py_upload_info_is_compressed_get, NULL, NULL, NULL},
    {0},
};

PyTypeObject pyUploadInfoType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_name = "pygl.textures.UploadInfo",
    .tp_basicsize = sizeof(PyUploadInfo),
    .tp_init = (initproc)py_upload_info_init,
    .tp_members = pyUploadInfoMembers,
    .tp_getset = pyUploadInfoGetSet,
};
