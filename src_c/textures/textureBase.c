#include "textureBase.h"
#include "../utility.h"
#include "uploadInfo.h"
#include <assert.h>

static size_t get_texture_format_size(GLenum format)
{
    switch (format)
    {
    case GL_RED:
    case GL_DEPTH_COMPONENT:
    case GL_STENCIL_INDEX:
        return 1;
    case GL_RG:
        return 2;
    case GL_RGB:
    case GL_BGR:
        return 3;
    case GL_RGBA:
    case GL_BGRA:
        return 4;
    }

    assert(0 && "Unreachable at textureBase::get_texture_format_size");
    return 0;
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

static void set_default_texture_parameters(GLuint tex, PyTextureSpec* spec)
{
    texture_set_parameter_int(tex, GL_TEXTURE_BASE_LEVEL, 0);
    texture_set_parameter_int(tex, GL_TEXTURE_MAX_LEVEL, max(spec->mipmaps - 1, 0));
    texture_set_parameter_int(tex, GL_TEXTURE_MIN_FILTER, (GLint)spec->minFilter);
    texture_set_parameter_int(tex, GL_TEXTURE_MAG_FILTER, (GLint)spec->magFilter);
    texture_set_parameter_int(tex, GL_TEXTURE_WRAP_S, (GLint)spec->wrapMode);
    texture_set_parameter_int(tex, GL_TEXTURE_WRAP_T, (GLint)spec->wrapMode);
    texture_set_parameter_int(tex, GL_TEXTURE_WRAP_R, (GLint)spec->wrapMode);
}

static bool check_texture_immutable_format(GLuint tex)
{
    GLint immutableFormat = GL_FALSE;
    glGetTextureParameteriv(tex, GL_TEXTURE_IMMUTABLE_FORMAT, &immutableFormat);
    if (!immutableFormat)
    {
        PyErr_SetString(PyExc_RuntimeError, "Couldn't create immutable texture storage.");
        return false;
    }

    return true;
}

static PyObject* delete(PyTexture* self, PyObject* Py_UNUSED(args))
{
    glDeleteTextures(1, &self->id);
    self->id = 0;
    Py_RETURN_NONE;
}

static PyObject* upload(PyTexture* self, PyObject* args, PyObject* kwargs)
{
    static char* kwNames[] = {"info", "data", NULL};

    PyObject* result = NULL;

    Py_buffer dataBuffer = {0};
    PyUploadInfo* info = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!y*", kwNames, &pyUploadInfoType, &info, &dataBuffer))
        goto end;

    if (info->width + info->xOffset > self->width || info->height + info->xOffset > self->height)
    {
        PyErr_SetString(PyExc_ValueError, "Both width and height with offsets added must be less or equal to the texture dimensions.");
        goto end;
    }

    if (!utils_check_buffer_contiguous(&dataBuffer))
        goto end;

    size_t dataStart = info->yOffset * info->width + info->xOffset;

    if (info->imageSize > 0)
    {
        if (info->imageSize + dataStart > dataBuffer.len)
        {
            PyErr_Format(PyExc_ValueError, "Image size exceeds buffer size with specified offset. (image size: %zu, buffer size: %zu)", info->imageSize, dataBuffer.len);
            goto end;
        }

        glCompressedTextureSubImage2D(
            self->id,
            info->level, info->xOffset, info->yOffset,
            info->width, info->height,
            info->format,
            info->imageSize,
            (char*)dataBuffer.buf + info->dataOffset);
    }
    else
    {
        size_t dataLen = info->width * info->height * get_pixel_type_size(info->type);
        if (dataStart + dataLen > dataBuffer.len)
        {
            PyErr_Format(PyExc_ValueError, "Calculated transfer data size exceeds buffer size with specified offset. (calculated size: %zu, buffer size: %zu)", dataLen, dataBuffer.len);
            goto end;
        }

        glTextureSubImage2D(
            self->id,
            info->level, info->xOffset, info->yOffset,
            info->width, info->height,
            info->format, info->type,
            (char*)dataBuffer.buf + info->dataOffset);
    }

    if (info->generateMipmap)
        glGenerateTextureMipmap(self->id);

    result = Py_NewRef(Py_None);
end:
    if (&dataBuffer.buf != NULL)
        PyBuffer_Release(&dataBuffer);

    return result;
}

static PyObject* bind(PyTexture* self, PyObject* Py_UNUSED(args))
{
    glBindTexture(self->bindTarget, self->id);
    Py_RETURN_NONE;
}

static PyObject* bind_to_unit(PyTexture* self, PyObject* unit)
{
    ThrowIf(
        !PyLong_Check(unit),
        PyExc_ValueError,
        "Texture unit has to be of type int",
        NULL);

    int _unit = PyLong_AsLong(unit);
    ThrowIf(
        _unit < 0,
        PyExc_ValueError,
        "Texture unit has to be greater than 0.",
        NULL);

    glBindTextureUnit((GLuint)_unit, self->id);
    Py_RETURN_NONE;
}

static PyObject* set_parameter(PyTexture* self, PyObject* args)
{
    GLenum param = 0;
    PyObject* value = NULL;
    if (!PyArg_ParseTuple(args, "IO", &param, &value))
        return NULL;

    if (PyLong_Check(value))
        texture_set_parameter_int(self->id, param, PyLong_AsLong(value));
    else if (PyFloat_Check(value))
        texture_set_parameter_float(self->id, param, (float)PyFloat_AS_DOUBLE(value));
    else
    {
        PyErr_SetString(PyExc_TypeError, "value has to be of type int or float.");
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject* generate_mipmap(PyTexture* self, PyObject* Py_UNUSED(args))
{
    glGenerateTextureMipmap(self->id);
    Py_RETURN_NONE;
}

void py_texture_dealloc(PyTexture* self)
{
    delete(self, NULL);
    Py_TYPE(self)->tp_free(self);
}

#pragma region Internal
bool texture_create(PyTexture* tex, GLenum target, PyTextureSpec* spec, bool setParameters)
{
    tex->bindTarget = target;
    tex->width = spec->width;
    tex->height = spec->height;
    tex->layers = spec->layers;

    glCreateTextures(target, 1, &tex->id);

    switch (target)
    {
    case GL_TEXTURE_1D:
        glTextureStorage1D(tex->id, spec->samples, spec->internalFormat, spec->width);
    case GL_TEXTURE_2D:
        glTextureStorage2D(tex->id, spec->mipmaps, spec->internalFormat, spec->width, spec->height);
        break;
    case GL_TEXTURE_2D_MULTISAMPLE:
        glTextureStorage2DMultisample(tex->id, spec->samples, spec->internalFormat, spec->width, spec->height, GL_TRUE);
        break;
    case GL_TEXTURE_2D_ARRAY:
        glTextureStorage3D(tex->id, spec->mipmaps, spec->internalFormat, spec->width, spec->height, spec->layers);
        break;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        glTextureStorage3DMultisample(tex->id, spec->samples, spec->internalFormat, spec->width, spec->height, spec->layers, GL_TRUE);
        break;
    default:
        PyErr_SetString(PyExc_ValueError, "Unsupported texture target encountered.");
        return false;
    }

    if (setParameters)
        set_default_texture_parameters(tex->id, spec);

    return check_texture_immutable_format(tex->id);
}

GLuint texture_create_fb_attachment(PyAttachmentSpec* spec, GLsizei width, GLsizei height)
{
    GLuint id = 0;
    GLenum target = spec->samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    glCreateTextures(target, 1, &id);

    if (spec->samples > 1)
        glTextureStorage2DMultisample(id, spec->samples, spec->format, width, height, GL_TRUE);
    else
        glTextureStorage2D(id, 1, spec->format, width, height);

    PyTextureSpec texSpec = {
        .minFilter = spec->minFilter,
        .magFilter = spec->magFilter,
        .mipmaps = 1,
        .wrapMode = GL_CLAMP_TO_EDGE,
    };
    set_default_texture_parameters(id, &texSpec);

    return id;
}

void texture_set_parameter_int(GLuint tex, GLenum parameter, GLint value)
{
    glTextureParameteri(tex, parameter, value);
}

void texture_set_parameter_float(GLuint tex, GLenum parameter, GLfloat value)
{
    glTextureParameterf(tex, parameter, value);
}
#pragma endregion

PyMethodDef pyTextureMethods[] = {
    {"delete", (PyCFunction)delete, METH_NOARGS, NULL},
    {"upload", (PyCFunction)upload, METH_VARARGS | METH_KEYWORDS, NULL},
    {"set_parameter", (PyCFunction)set_parameter, METH_VARARGS, NULL},
    {"bind", (PyCFunction)bind, METH_NOARGS, NULL},
    {"bind_to_unit", (PyCFunction)bind_to_unit, METH_O, NULL},
    {"generate_mipmap", (PyCFunction)generate_mipmap, METH_NOARGS, NULL},
    {0},
};
