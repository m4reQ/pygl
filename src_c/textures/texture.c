#include <stdbool.h>
#include "texture.h"
#include "textureSpec.h"
#include "uploadInfo.h"

static const char *target_enum_to_string(GLenum target)
{
    switch (target)
    {
    case GL_TEXTURE_1D:
        return "TEXTURE_1D";
    case GL_TEXTURE_2D:
        return "TEXTURE_2D";
    case GL_TEXTURE_3D:
        return "TEXTURE_3D";
    case GL_TEXTURE_1D_ARRAY:
        return "TEXTURE_1D_ARRAY";
    case GL_TEXTURE_2D_ARRAY:
        return "TEXTURE_2D_ARRAY";
    case GL_TEXTURE_RECTANGLE:
        return "TEXTURE_RECTANGLE";
    case GL_TEXTURE_CUBE_MAP:
        return "TEXTURE_CUBE_MAP";
    case GL_TEXTURE_CUBE_MAP_ARRAY:
        return "TEXTURE_CUBE_MAP_ARRAY";
    case GL_TEXTURE_BUFFER:
        return "TEXTURE_BUFFER";
    case GL_TEXTURE_2D_MULTISAMPLE:
        return "TEXTURE_2D_MULTISAMPLE";
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        return "TEXTURE_2D_MULTISAMPLE_ARRAY";
    }

    assert(false && "Unreachable at texture.c:target_enum_to_string");
    return NULL;
}

static size_t get_pixel_type_size(GLenum type)
{
    switch (type)
    {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
    case GL_UNSIGNED_BYTE_3_3_2:
    case GL_UNSIGNED_BYTE_2_3_3_REV:
        return sizeof(GLbyte);
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_5_6_5_REV:
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
        return sizeof(GLshort);
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_UNSIGNED_INT_8_8_8_8:
    case GL_UNSIGNED_INT_8_8_8_8_REV:
    case GL_UNSIGNED_INT_10_10_10_2:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
        return sizeof(GLint);
    case GL_FLOAT:
        return sizeof(GLfloat);
    }

    assert(0 && "Unreachable at textureBase::get_pixel_type_size");
    return 0;
}

static size_t get_data_offset(const PyUploadInfo *info)
{
    if (info->dataOffset > 0)
        return info->dataOffset;

    size_t xOffset = info->xOffset;
    size_t yOffset = info->yOffset * info->width;
    size_t zOffset = info->zOffset * info->width * info->height;
    return (xOffset + yOffset + zOffset) * get_pixel_type_size(info->pixelType);
}

static size_t get_data_length(const PyUploadInfo *info)
{
    if (info->imageSize > 0)
        return info->imageSize;

    return info->width * info->height * info->depth * get_pixel_type_size(info->pixelType);
}

static void *get_data_ptr(const Py_buffer *buffer, const PyUploadInfo *info)
{
    if (!PyBuffer_IsContiguous(buffer, 'c'))
    {
        PyErr_SetString(PyExc_BufferError, "Data buffer has to be C-contiguous. For more informations go to https://github.com/m4reQ/pygl?tab=readme-ov-file#buffer-protocol-usage.");
        return NULL;
    }

    size_t offsetBytes = get_data_offset(info);
    size_t lengthBytes = get_data_length(info);

    if (offsetBytes + lengthBytes > buffer->len)
    {
        PyErr_Format(
            PyExc_BufferError,
            "Requested transfer data size exceeds provided buffer size (offset: %zu, calculated: %zu, provided: %zu).",
            offsetBytes,
            lengthBytes,
            buffer->len);
        return NULL;
    }

    return (char *)buffer->buf + offsetBytes;
}

static bool upload_texture_1d(PyTexture *texture, const PyUploadInfo *info, const Py_buffer *buffer)
{
    if (info->width <= 0 || info->xOffset <= 0)
    {
        PyErr_SetString(PyExc_ValueError, "1D texture upload requires upload info width and x_offset to be non-negative.");
        return false;
    }

    const void *dataPtr = get_data_ptr(buffer, info);
    if (dataPtr == NULL)
        return false;

    if (info->imageSize != 0)
        glCompressedTextureSubImage1D(
            texture->id,
            info->level,
            info->xOffset,
            info->width,
            info->format,
            info->imageSize,
            dataPtr);
    else
        glTextureSubImage1D(
            texture->id,
            info->level,
            info->xOffset,
            info->width,
            info->format,
            info->pixelType,
            dataPtr);

    return true;
}

static bool upload_texture_2d(PyTexture *texture, const PyUploadInfo *info, const Py_buffer *buffer)
{
    if (info->width <= 0 ||
        info->xOffset <= 0 ||
        info->height <= 0 ||
        info->yOffset <= 0)
    {
        PyErr_SetString(PyExc_ValueError, "2D texture upload requires width, x_offset, height and y_offset to be non-negative.");
        return false;
    }

    const void *dataPtr = get_data_ptr(buffer, info);
    if (dataPtr == NULL)
        return false;

    if (info->imageSize != 0)
        glCompressedTextureSubImage2D(
            texture->id,
            info->level,
            info->xOffset,
            info->yOffset,
            info->width,
            info->height,
            info->format,
            info->imageSize,
            dataPtr);
    else
        glTextureSubImage2D(
            texture->id,
            info->level,
            info->xOffset,
            info->yOffset,
            info->width,
            info->height,
            info->format,
            info->pixelType,
            dataPtr);

    return true;
}

static bool upload_texture_3d(PyTexture *texture, const PyUploadInfo *info, const Py_buffer *buffer)
{
    if (info->width <= 0 ||
        info->xOffset <= 0 ||
        info->height <= 0 ||
        info->yOffset <= 0 ||
        info->depth <= 0 ||
        info->zOffset <= 0)
    {
        PyErr_SetString(PyExc_ValueError, "3D texture upload requires width, x_offset, height, y_offset, depth and z_offset to be non-negative.");
        return false;
    }

    const void *dataPtr = get_data_ptr(buffer, info);
    if (dataPtr == NULL)
        return false;

    if (info->imageSize != 0)
        glCompressedTextureSubImage3D(
            texture->id,
            info->level,
            info->xOffset,
            info->yOffset,
            info->zOffset,
            info->width,
            info->height,
            info->depth,
            info->format,
            info->imageSize,
            dataPtr);
    else
        glTextureSubImage3D(
            texture->id,
            info->level,
            info->xOffset,
            info->yOffset,
            info->zOffset,
            info->width,
            info->height,
            info->depth,
            info->format,
            info->pixelType,
            dataPtr);

    return true;
}

static bool create_1d_texture_storage(const PyTexture *texture, const PyTextureSpec *spec)
{
    if (texture->width <= 0)
    {
        PyErr_SetString(PyExc_ValueError, "Texture width must be positive for 1D texture.");
        return false;
    }

    glTextureStorage1D(texture->id, spec->mipmaps, spec->internalFormat, texture->width);
    return true;
}

static bool create_2d_texture_storage(const PyTexture *texture, const PyTextureSpec *spec)
{
    if (texture->width <= 0 || texture->height <= 0)
    {
        PyErr_SetString(PyExc_ValueError, "Texture width and height must be positive for 2D texture or 1D texture array.");
        return false;
    }

    if (texture->target == GL_TEXTURE_2D_MULTISAMPLE)
        glTextureStorage2DMultisample(
            texture->id,
            spec->samples,
            spec->internalFormat,
            texture->width,
            texture->height,
            GL_TRUE);
    else
        glTextureStorage2D(
            texture->id,
            texture->mipmaps,
            spec->internalFormat,
            texture->width,
            texture->height);

    return true;
}

static bool create_3d_texture_storage(const PyTexture *texture, const PyTextureSpec *spec)
{
    if (texture->width < 0 || texture->height < 0 || texture->depth < 0)
    {
        PyErr_SetString(PyExc_ValueError, "Texture width, height and depth must be positive for 3D texture, 2D array texture or cubemap texture.");
        return false;
    }

    if (texture->target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
        glTextureStorage3DMultisample(
            texture->id,
            spec->samples,
            spec->internalFormat,
            texture->width,
            texture->height,
            texture->depth,
            GL_TRUE);
    else
        glTextureStorage3D(
            texture->id,
            texture->mipmaps,
            spec->internalFormat,
            texture->width,
            texture->height,
            texture->depth);

    return true;
}

static bool check_texture_immutable_format(const PyTexture *texture)
{
    GLint immutableFormat = GL_FALSE;
    glGetTextureParameteriv(texture->id, GL_TEXTURE_IMMUTABLE_FORMAT, &immutableFormat);
    if (!immutableFormat)
    {
        PyErr_SetString(PyExc_RuntimeError, "Couldn't create immutable texture storage.");
        return false;
    }

    return true;
}

static PyObject *repr(PyTexture *self)
{
    return PyUnicode_FromFormat(
        "<%s object at %p [target: %s, width: %d, height: %d, depth: %d, mipmaps: %d]>",
        Py_TYPE(self)->tp_name,
        self,
        target_enum_to_string(self->target),
        self->width,
        self->height,
        self->depth,
        self->mipmaps);
}

static void dealloc(PyTexture *self)
{
    glDeleteTextures(1, &self->id);
    Py_TYPE(self)->tp_free(self);
}

static int init(PyTexture *self, PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {"spec", "set_parameters", NULL};

    PyTextureSpec *spec;
    bool setParameters = false;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!|p", kwNames, &pyTextureSpecType, &spec, &setParameters))
        return -1;

    self->width = spec->width;
    self->height = spec->height;
    self->depth = self->target == GL_TEXTURE_CUBE_MAP ? 6 : spec->depth;
    self->target = spec->target;
    self->mipmaps = spec->mipmaps;

    if (self->mipmaps < 0)
    {
        PyErr_SetString(PyExc_ValueError, "Mipmaps count for texture must be positive.");
        return -1;
    }

    glCreateTextures(self->target, 1, &self->id);

    bool storageCreateSuccess = false;
    if (self->target == GL_TEXTURE_1D)
        storageCreateSuccess = create_1d_texture_storage(self, spec);
    else if (self->target == GL_TEXTURE_1D_ARRAY ||
             self->target == GL_TEXTURE_2D ||
             self->target == GL_TEXTURE_2D_MULTISAMPLE)
        storageCreateSuccess = create_2d_texture_storage(self, spec);
    else if (self->target == GL_TEXTURE_2D_ARRAY ||
             self->target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY ||
             self->target == GL_TEXTURE_3D ||
             self->target == GL_TEXTURE_CUBE_MAP)
        storageCreateSuccess = create_3d_texture_storage(self, spec);

    if (!storageCreateSuccess)
        return -1;

    if (setParameters)
    {
        glTextureParameteri(self->id, GL_TEXTURE_BASE_LEVEL, 0);
        glTextureParameteri(self->id, GL_TEXTURE_MAX_LEVEL, spec->mipmaps - 1);
        glTextureParameteri(self->id, GL_TEXTURE_MIN_FILTER, (GLint)spec->minFilter);
        glTextureParameteri(self->id, GL_TEXTURE_MAG_FILTER, (GLint)spec->magFilter);
        glTextureParameteri(self->id, GL_TEXTURE_WRAP_S, (GLint)spec->wrapMode);
        glTextureParameteri(self->id, GL_TEXTURE_WRAP_R, (GLint)spec->wrapMode);
        glTextureParameteri(self->id, GL_TEXTURE_WRAP_T, (GLint)spec->wrapMode);
    }

    if (!check_texture_immutable_format(self))
        return -1;

    return 0;
}

static PyObject *delete(PyTexture *self, PyObject *Py_UNUSED(args))
{
    glDeleteTextures(1, &self->id);
    self->id = 0;

    Py_RETURN_NONE;
}

static PyObject *bind(PyTexture *self, PyObject *Py_UNUSED(args))
{
    glBindTexture(self->target, self->id);
    Py_RETURN_NONE;
}

static PyObject *bind_to_unit(PyTexture *self, PyObject *unit)
{
    if (!PyLong_Check(unit))
    {
        PyErr_Format(PyExc_TypeError, "Expected argument to be of type int, got: %s.", Py_TYPE(unit)->tp_name);
        return NULL;
    }

    GLint unitInt = PyLong_AsLong(unit);
    if (unitInt < 0)
    {
        PyErr_SetString(PyExc_ValueError, "Texture unit must be non-negative.");
        return NULL;
    }

    glBindTextureUnit((GLuint)unitInt, self->id);

    Py_RETURN_NONE;
}

static PyObject *set_parameter(PyTexture *self, PyObject *args)
{
    GLenum parameterName;
    PyObject *value;

    if (!PyArg_ParseTuple(args, "IO", &parameterName, &value))
        return NULL;

    if (PyLong_Check(value))
        glTextureParameteri(self->id, parameterName, PyLong_AsLong(value));
    else if (PyFloat_Check(value))
        glTextureParameterf(self->id, parameterName, (float)PyFloat_AS_DOUBLE(value));
    else
    {
        PyErr_Format(PyExc_TypeError, "Expected argument 2 to be of type int or float, got: %s.", Py_TYPE(value)->tp_name);
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *generate_mipmap(PyTexture *self, PyObject *Py_UNUSED(args))
{
    glGenerateTextureMipmap(self->id);
    Py_RETURN_NONE;
}

static PyObject *upload(PyTexture *self, PyObject *args)
{
    PyObject *result = NULL;

    PyUploadInfo *info;
    Py_buffer buffer = {0};
    if (!PyArg_ParseTuple(args, "O!y*", &pyUploadInfoType, &info, &buffer))
        return NULL;

    bool uploadSuccess = false;
    if (self->target == GL_TEXTURE_1D)
        uploadSuccess = upload_texture_1d(self, info, &buffer);
    else if (self->target == GL_TEXTURE_1D_ARRAY ||
             self->target == GL_TEXTURE_2D ||
             self->target == GL_TEXTURE_2D_MULTISAMPLE)
        uploadSuccess = upload_texture_2d(self, info, &buffer);
    else if (self->target == GL_TEXTURE_3D ||
             self->target == GL_TEXTURE_2D_ARRAY ||
             self->target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY ||
             self->target == GL_TEXTURE_CUBE_MAP)
        uploadSuccess = upload_texture_3d(self, info, &buffer);
    else
    {
        // TODO Implement support for array cubemap textures
        PyErr_SetString(PyExc_NotImplementedError, "Support for array cubemap textures is not implemented yet.");
        goto end;
    }

    if (!uploadSuccess)
        goto end;

    if (info->generateMipmap)
        glGenerateTextureMipmap(self->id);

    result = Py_NewRef(Py_None);

end:
    if (buffer.buf != NULL)
        PyBuffer_Release(&buffer);

    return result;
}

static PyObject *get_is_cubemap(PyTexture *self, void *Py_UNUSED(closure))
{
    return PyBool_FromLong(self->target == GL_TEXTURE_CUBE_MAP);
}

static PyObject *get_is_array(PyTexture *self, void *Py_UNUSED(closure))
{
    return PyBool_FromLong(
        self->target == GL_TEXTURE_1D_ARRAY ||
        self->target == GL_TEXTURE_2D_ARRAY ||
        self->target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY);
}

static PyObject *get_is_1d(PyTexture *self, void *Py_UNUSED(closure))
{
    return PyBool_FromLong(
        self->target == GL_TEXTURE_1D ||
        self->target == GL_TEXTURE_1D_ARRAY);
}

static PyObject *get_is_2d(PyTexture *self, void *Py_UNUSED(closure))
{
    return PyBool_FromLong(
        self->target == GL_TEXTURE_2D ||
        self->target == GL_TEXTURE_2D_ARRAY ||
        self->target == GL_TEXTURE_2D_MULTISAMPLE ||
        self->target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY);
}

static PyObject *get_is_3d(PyTexture *self, void *Py_UNUSED(closure))
{
    return PyBool_FromLong(self->target == GL_TEXTURE_3D);
}

PyTypeObject pyTextureType = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "pygl.textures.Texture",
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_basicsize = sizeof(PyTexture),
    .tp_new = PyType_GenericNew,
    .tp_repr = (reprfunc)repr,
    .tp_dealloc = (destructor)dealloc,
    .tp_init = (initproc)init,
    .tp_members = (PyMemberDef[]){
        {"id", Py_T_UINT, offsetof(PyTexture, id), Py_READONLY, NULL},
        {"target", Py_T_UINT, offsetof(PyTexture, target), Py_READONLY, NULL},
        {"width", Py_T_INT, offsetof(PyTexture, width), Py_READONLY, NULL},
        {"height", Py_T_INT, offsetof(PyTexture, height), Py_READONLY, NULL},
        {"depth", Py_T_INT, offsetof(PyTexture, depth), Py_READONLY, NULL},
        {"mipmaps", Py_T_INT, offsetof(PyTexture, mipmaps), Py_READONLY, NULL},
        {0},
    },
    .tp_methods = (PyMethodDef[]){
        {"delete", (PyCFunction) delete, METH_NOARGS, NULL},
        {"bind", (PyCFunction)bind, METH_NOARGS, NULL},
        {"bind_to_unit", (PyCFunction)bind_to_unit, METH_O, NULL},
        {"upload", (PyCFunction)upload, METH_VARARGS, NULL},
        {"set_parameter", (PyCFunction)set_parameter, METH_VARARGS, NULL},
        {"generate_mipmap", (PyCFunction)generate_mipmap, METH_NOARGS, NULL},
        {0},
    },
    .tp_getset = (PyGetSetDef[]){
        {"is_cubemap", (getter)get_is_cubemap, NULL, NULL, NULL},
        {"is_array", (getter)get_is_array, NULL, NULL, NULL},
        {"id_1d", (getter)get_is_1d, NULL, NULL, NULL},
        {"is_2d", (getter)get_is_2d, NULL, NULL, NULL},
        {"is_3d", (getter)get_is_3d, NULL, NULL, NULL},
        {0},
    },
};

GLuint texture_create_fb_attachment(PyAttachmentSpec *spec, GLsizei width, GLsizei height)
{
    GLuint id = 0;
    GLenum target = spec->samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    glCreateTextures(target, 1, &id);

    if (spec->samples > 1)
        glTextureStorage2DMultisample(id, spec->samples, spec->format, width, height, GL_TRUE);
    else
        glTextureStorage2D(id, 1, spec->format, width, height);

    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, spec->minFilter);
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, spec->magFilter);
    glTextureParameteri(id, GL_TEXTURE_MAX_LEVEL, 1);
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return id;
}
