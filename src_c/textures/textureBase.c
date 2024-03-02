#include "textureBase.h"
#include "../utility.h"
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

PyObject* py_texture_delete(PyTexture* self, PyObject* Py_UNUSED(args))
{
    glDeleteTextures(1, &self->id);
    self->id = 0;
    Py_RETURN_NONE;
}

PyObject* py_texture_upload(PyTexture* self, PyObject* args, PyObject* kwargs)
{
    static char* kwNames[] = {"width", "height", "format", "data", /* optional */ "type", "level", "x_offset", "y_offset", NULL};

    PyObject* result = NULL;

    GLsizei width = 0, height = 0;
    Py_buffer dataBuffer = {0};
    GLenum format = 0;

    GLint level = 0;
    GLint xOffset = 0, yOffset = 0;
    GLenum type = GL_UNSIGNED_BYTE;
    if (!PyArg_ParseTupleAndKeywords(
        args, kwargs, "iiIy*|Iiii", kwNames,
        &width, &height, &format, &dataBuffer,
        &type, &level, &xOffset, &yOffset))
        goto end;

    if (width < 0 || height < 0)
    {
        PyErr_SetString(PyExc_ValueError, "Both width and height must be greater than 0.");
        goto end;
    }

    if (width + xOffset > self->width || height + xOffset > self->height)
    {
        PyErr_SetString(PyExc_ValueError, "Both width and height with offsets added must be less or equal to the texture dimensions.");
        goto end;
    }

    if (!PyBuffer_IsContiguous(&dataBuffer, 'C'))
    {
        PyErr_SetString(PyExc_ValueError, "Data buffer has to be C-contiguous. For more informations go to https://github.com/m4reQ/pygl?tab=readme-ov-file#buffer-protocol-usage.");
        goto end;
    }

    size_t dataStart = yOffset * width + xOffset;
    size_t dataLen = width * height * get_pixel_type_size(type);
    if (dataStart + dataLen > dataBuffer.len)
    {
        PyErr_Format(PyExc_ValueError, "Calculated transfer data size exceeds buffer size with specified offset. (calculated size: %zu, buffer size: %zu)", dataLen, dataBuffer.len);
        goto end;
    }

    glTextureSubImage2D(self->id, level, xOffset, yOffset, width, height, format, type, dataBuffer.buf);

    result = Py_NewRef(Py_None);
end:
    if (&dataBuffer.buf != NULL)
        PyBuffer_Release(&dataBuffer);

    return result;
}

PyObject* py_texture_bind(PyTexture* self, PyObject* Py_UNUSED(args))
{
    glBindTexture(self->bindTarget, self->id);
    Py_RETURN_NONE;
}

PyObject* py_texture_bind_to_unit(PyTexture* self, PyObject* unit)
{
    ThrowIf(
        !PyLong_Check(unit),
        PyExc_ValueError,
        "Texture unit has to be of type int",
        NULL);

    // TODO Maybe check if unit is not less than 0
    glBindTextureUnit(PyLong_AsUnsignedLong(unit), self->id);
    Py_RETURN_NONE;
}

PyObject* py_texture_bind_textures(PyObject* Py_UNUSED(cls), PyObject* args, PyObject* kwargs)
{
    static char* kwNames[] = {"textures", "first", NULL};

    GLuint first = 0;
    PyObject* textures = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!|I", kwNames, &PyList_Type, &textures, &first))
        return NULL;

    GLsizei nTextures = PyList_GET_SIZE(textures);
    if (nTextures == 0)
        Py_RETURN_NONE;

    GLuint* textureIds = PyMem_Malloc(sizeof(GLuint) * nTextures);
    for (GLsizei i = 0; i < nTextures; i++)
    {
        PyTexture* tex = (PyTexture*)PyList_GET_ITEM(textures, i);
        textureIds[i] = tex->id;
    }

    glBindTextures(first, nTextures, textureIds);

    PyMem_Free(textureIds);
    Py_RETURN_NONE;
}

void py_texture_dealloc(PyTexture* self)
{
    py_texture_delete(self, NULL);
    Py_TYPE(self)->tp_free(self);
}

PyObject* py_texture_set_parameter(PyTexture* self, PyObject* args)
{
    GLenum param = 0;
    PyObject* value = NULL;
    if (!PyArg_ParseTuple(args, "IO"))
        return NULL;

    if (PyLong_Check(value))
        texture_set_parameter_int(self, param, PyLong_AsLong(value));
    else if (PyFloat_Check(value))
        texture_set_parameter_float(self, param, (float)PyFloat_AS_DOUBLE(value));
    else
    {
        PyErr_SetString(PyExc_TypeError, "value has to be of type int or float.");
        return NULL;
    }

    Py_RETURN_NONE;
}

bool texture_create(PyTexture* tex, GLenum target, PyTextureSpec* spec, bool setParameters)
{
    tex->bindTarget = target;
    tex->width = spec->width;
    tex->height = spec->height;

    glCreateTextures(target, 1, &tex->id);

    switch (target)
    {
    case GL_TEXTURE_1D:
        glTextureStorage1D(tex->id, spec->samples, spec->internalFormat, spec->width);
    case GL_TEXTURE_2D:
        if (spec->samples > 1)
            glTextureStorage2DMultisample(tex->id, spec->samples, spec->internalFormat, spec->width, spec->height, GL_TRUE);
        else
            glTextureStorage2D(tex->id, spec->mipmaps, spec->internalFormat, spec->width, spec->height);
        break;
    default:
        PyErr_SetString(PyExc_ValueError, "Unsupported texture target encountered.");
        return false;
    }

    if (setParameters)
    {
        texture_set_parameter_int(tex, GL_TEXTURE_BASE_LEVEL, 0);
        texture_set_parameter_int(tex, GL_TEXTURE_MAX_LEVEL, max(spec->mipmaps - 1, 0));
        texture_set_parameter_int(tex, GL_TEXTURE_MIN_FILTER, (GLint)spec->minFilter);
        texture_set_parameter_int(tex, GL_TEXTURE_MAG_FILTER, (GLint)spec->magFilter);
        texture_set_parameter_int(tex, GL_TEXTURE_WRAP_S, (GLint)spec->wrapMode);
        texture_set_parameter_int(tex, GL_TEXTURE_WRAP_T, (GLint)spec->wrapMode);
        texture_set_parameter_int(tex, GL_TEXTURE_WRAP_R, (GLint)spec->wrapMode);
    }

    GLint immutableFormat = GL_FALSE;
    glGetTextureParameteriv(tex->id, GL_TEXTURE_IMMUTABLE_FORMAT, &immutableFormat);
    if (!immutableFormat)
    {
        PyErr_SetString(PyExc_RuntimeError, "Couldn't create immutable texture storage.");
        return false;
    }

    return true;
}

void texture_set_parameter_int(PyTexture* tex, GLenum parameter, GLint value)
{
    glTextureParameteri(tex->id, parameter, value);
}

void texture_set_parameter_float(PyTexture* tex, GLenum parameter, GLfloat value)
{
    glTextureParameterf(tex->id, parameter, value);
}
