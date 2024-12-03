#include <stdbool.h>
#include <stdio.h>
#include "shader.h"
#include "shaderStage.h"
#include "../utility.h"

#define READ_FILE_CHUNK_SIZE 1024 * 64 // 64 KiB
#define SET_UNIFORM_MAT_IMPL(type)                                                    \
    switch (len)                                                                      \
    {                                                                                 \
    case 2:                                                                           \
        glProgramUniformMatrix2##type##v(id, loc, 1, (GLboolean)transpose, buf->buf); \
        break;                                                                        \
    case 3:                                                                           \
        glProgramUniformMatrix3##type##v(id, loc, 1, (GLboolean)transpose, buf->buf); \
        break;                                                                        \
    case 4:                                                                           \
        glProgramUniformMatrix4##type##v(id, loc, 1, (GLboolean)transpose, buf->buf); \
        break;                                                                        \
    default:                                                                          \
        PyErr_SetString(PyExc_ValueError, "Invalid matrix length provided.");         \
        return false;                                                                 \
    }                                                                                 \
    return true;

typedef struct
{
    GLint location;
    GLint nameLength;
} ResourceProto;

static GLint get_uniform_location(PyShader *shader, PyObject *name)
{
    PyObject *locationObj = PyDict_GetItem(shader->resources, name);
    if (!locationObj)
    {
        PyErr_Format(PyExc_ValueError, "Couldn't find uniform named %U.", name);
        return -1;
    }

    return PyLong_AsLong(locationObj);
}

static bool check_compile_success(GLuint stage)
{
    GLint compileSuccess = GL_FALSE;
    glGetShaderiv(stage, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess)
        return true;

    GLint logLength = 0;
    glGetShaderiv(stage, GL_INFO_LOG_LENGTH, &logLength);

    const size_t bufSize = sizeof(char) * logLength;
    char *infoLog = PyMem_Malloc(bufSize);
    glGetShaderInfoLog(stage, bufSize, NULL, infoLog);

    PyErr_Format(PyExc_RuntimeError, "Shader stage compilation failure:\n%s", infoLog);
    PyMem_Free(infoLog);
    return false;
}

static bool check_link_success(GLuint shader)
{
    GLint linkSuccess = GL_FALSE;
    glGetProgramiv(shader, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess)
        return true;

    GLint logLength = 0;
    glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &logLength);

    const size_t bufferSize = sizeof(char) * logLength;
    char *infoLog = PyMem_Malloc(bufferSize);
    glGetProgramInfoLog(shader, bufferSize, NULL, infoLog);

    PyErr_Format(PyExc_RuntimeError, "Shader link failure:\n%s", infoLog);
    PyMem_Free(infoLog);

    return false;
}

static bool read_whole_file(FILE *file, char **outData, size_t *outSize)
{
    char *buffer = NULL;
    size_t read = 0;
    size_t bufSize = 0;

    while (true)
    {
        if (read + READ_FILE_CHUNK_SIZE + 1 > bufSize)
        {
            bufSize = read + READ_FILE_CHUNK_SIZE + 1;
            buffer = PyMem_Realloc(buffer, bufSize);
        }

        size_t readCurrent = fread_s(buffer, bufSize, sizeof(char), bufSize, file);
        if (readCurrent < READ_FILE_CHUNK_SIZE)
        {
            read += readCurrent;
            break;
        }

        read += readCurrent;
    }

    *outData = realloc(buffer, read + 1);
    (*outData)[read] = '\0';
    *outSize = read;

    return true;
}

static const char *get_shader_stage_source(PyShaderStage *stage, size_t *sourceLength)
{
    if (stage->fromSource)
        return PyUnicode_AsUTF8AndSize(stage->pyFilepath, sourceLength);

    FILE *sourceFile = NULL;
    THROW_IF(
        fopen_s(&sourceFile, PyUnicode_AsUTF8(stage->pyFilepath), "r"),
        PyExc_RuntimeError,
        "Couldn't open shader stage source file.",
        NULL);

    char *source = NULL;
    THROW_IF(
        !read_whole_file(sourceFile, &source, sourceLength),
        PyExc_RuntimeError,
        "Couldn't read shader stage source file.",
        NULL);

    return source;
}

static GLuint create_stage(PyShaderStage *stage)
{
    size_t sourceLength = 0;
    const char *source = get_shader_stage_source(stage, &sourceLength);

    GLuint id = glCreateShader(stage->type);
    glShaderSource(id, 1, &source, (GLint *)&sourceLength);
    glCompileShader(id);

    if (!stage->fromSource)
        PyMem_Free((char *)source);

    if (!check_compile_success(id))
        return -1;

    return id;
}

static void retrieve_interface(GLuint shader, GLenum interface, PyObject *storage)
{
    GLint resourceCount = 0;
    glGetProgramInterfaceiv(shader, interface, GL_ACTIVE_RESOURCES, &resourceCount);

    for (GLint i = 0; i < resourceCount; i++)
    {
        static const GLenum properties[] = {GL_LOCATION, GL_NAME_LENGTH};

        ResourceProto proto;
        glGetProgramResourceiv(
            shader,
            interface,
            i,
            _countof(properties),
            properties,
            sizeof(ResourceProto),
            NULL,
            &proto.location);

        // skip members of buffer uniform blocks
        if (proto.location == -1)
            continue;

        const size_t bufSize = sizeof(char) * (proto.nameLength + 1);
        char *name = PyMem_Malloc(bufSize);
        glGetProgramResourceName(shader, interface, i, bufSize, NULL, name);
        name[bufSize - 1] = '\0';

        // normalize array names (array[0] -> array)
        char *bracket = strchr(name, '[');
        if (bracket)
            *bracket = '\0';

        PyDict_SetItemString(storage, name, PyLong_FromLong(proto.location));
    }
}

static bool is_index_valid(PyObject *index)
{
    return PyTuple_Check(index) &&
           PyTuple_GET_SIZE(index) == 2 &&
           PyUnicode_Check(PyTuple_GET_ITEM(index, 0)) &&
           PyLong_Check(PyTuple_GET_ITEM(index, 1));
}

static int py_shader_init(PyShader *self, PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {"stages", "attrib_indices", NULL};

    PyObject *stages;
    PyObject *attribIndices = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!|O!", kwNames, &PyList_Type, &stages, &PyList_Type, &attribIndices))
        return -1;

    self->id = glCreateProgram();
    self->resources = PyDict_New();

    Py_ssize_t nStages = PyList_GET_SIZE(stages);
    GLuint *attachedStages = PyMem_Malloc(sizeof(GLuint) * nStages);

    for (Py_ssize_t i = 0; i < nStages; i++)
    {
        PyShaderStage *_stage = (PyShaderStage *)PyList_GET_ITEM(stages, i);
        THROW_IF(
            !PyObject_IsInstance((PyObject *)_stage, (PyObject *)&pyShaderStageType),
            PyExc_TypeError,
            "Second argument must be a list of pygl.shaders.ShaderStage objects",
            -1);

        GLuint stage = create_stage(_stage);
        if (stage == -1)
            // exception already set by create_stage
            return -1;

        glAttachShader(self->id, stage);
        attachedStages[i] = stage;
    }

    if (attribIndices)
    {
        size_t attribIndicesCount = PyList_GET_SIZE(attribIndices);
        for (size_t i = 0; i < attribIndicesCount; i++)
        {
            PyObject *index = PyList_GET_ITEM(attribIndices, i);
            if (!is_index_valid(index))
            {
                PyErr_Format(PyExc_TypeError, "Elements of attrib_indices must be tuples of (str, int), got: %s (at location: %d).", Py_TYPE(index)->tp_name, i);
                return -1;
            }

            const char *attribName = PyUnicode_AsUTF8(PyTuple_GET_ITEM(index, 0));
            GLuint location = PyLong_AsUnsignedLong(PyTuple_GET_ITEM(index, 1));

            glBindAttribLocation(self->id, location, attribName);
        }
    }

    glLinkProgram(self->id);
    if (!check_link_success(self->id))
        return -1;

    retrieve_interface(self->id, GL_PROGRAM_INPUT, self->resources);
    retrieve_interface(self->id, GL_UNIFORM, self->resources);

    // cleanup
    for (Py_ssize_t i = 0; i < nStages; i++)
    {
        glDetachShader(self->id, attachedStages[i]);
        glDeleteShader(attachedStages[i]);
    }

    PyMem_Free(attachedStages);

    return 0;
}

static PyObject *py_shader_delete(PyShader *self, PyObject *Py_UNUSED(args))
{
    glDeleteProgram(self->id);
    self->id = 0;

    Py_RETURN_NONE;
}

static void py_shader_dealloc(PyShader *self)
{
    py_shader_delete(self, NULL);
    Py_CLEAR(self->resources);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *py_shader_use(PyShader *self, PyObject *Py_UNUSED(args))
{
    glUseProgram(self->id);
    Py_RETURN_NONE;
}

static PyObject *py_shader_get_resource_location(PyShader *self, PyObject *name)
{
    THROW_IF(
        !PyUnicode_Check(name),
        PyExc_TypeError,
        "Name has to be of type str.",
        NULL);

    PyObject *location = PyDict_GetItem(self->resources, name);
    if (!location)
        PyErr_Format(PyExc_RuntimeError, "Couldn't find shader resource with name: %U.", name);

    return location;
}

static PyObject *py_shader_validate(PyShader *self, PyObject *Py_UNUSED(args))
{
    glValidateProgram(self->id);

    GLint success = GL_FALSE;
    glGetProgramiv(self->id, GL_VALIDATE_STATUS, &success);
    if (!success)
    {
        GLint logLength = 0;
        glGetProgramiv(self->id, GL_INFO_LOG_LENGTH, &logLength);

        char *buffer = PyMem_Malloc(sizeof(char) * logLength);
        glGetProgramInfoLog(self->id, sizeof(char) * logLength, NULL, buffer);

        PyErr_Format(PyExc_RuntimeError, "Shader %d validation error:\n%s.", self->id, buffer);
        PyMem_Free(buffer);

        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *py_shader_get_uniform_block_location(PyShader *self, PyObject *name)
{
    THROW_IF(
        !PyUnicode_Check(name),
        PyExc_TypeError,
        "name has to be of type str.",
        NULL);

    GLuint index = glGetUniformBlockIndex(self->id, PyUnicode_AsUTF8(name));
    return PyLong_FromUnsignedLong(index);
}

static PyObject *py_shader_set_uniform_block_binding(PyShader *self, PyObject *args)
{
    char *name = NULL;
    GLuint binding = 0;
    if (!PyArg_ParseTuple(args, "sI", &name, &binding))
        return NULL;

    glUniformBlockBinding(self->id, glGetUniformBlockIndex(self->id, name), binding);
    Py_RETURN_NONE;
}

static PyObject *py_shader_set_uniform(PyShader *self, PyObject *args)
{
    static char *kwNames[] = {"name", "value", "type", NULL};

    PyObject *name = NULL;
    PyObject *value = NULL;
    GLenum type = GL_FLOAT;
    if (!PyArg_ParseTuple(args, "UO|I", &name, &value, &type))
        return NULL;

    GLint uniformLocation = get_uniform_location(self, name);
    if (uniformLocation == -1)
        return NULL; // exception already set

    switch (type)
    {
    case GL_FLOAT:
    {
        if (!PyFloat_Check(value))
        {
            PyErr_Format(PyExc_TypeError, "Expected value to be of type float, got: %s.", Py_TYPE(value)->tp_name);
            return NULL;
        }

        glProgramUniform1f(self->id, uniformLocation, (float)PyFloat_AS_DOUBLE(value));
        break;
    }

    case GL_DOUBLE:
    {
        if (!PyFloat_Check(value))
        {
            PyErr_Format(PyExc_TypeError, "Expected value to be of type float, got: %s.", Py_TYPE(value)->tp_name);
            return NULL;
        }

        glProgramUniform1d(self->id, uniformLocation, PyFloat_AS_DOUBLE(value));
        break;
    }
    case GL_INT:
    {
        if (!PyLong_Check(value))
        {
            PyErr_Format(PyExc_TypeError, "Expected value to be of type int, got: %s.", Py_TYPE(value)->tp_name);
            return NULL;
        }

        glProgramUniform1i(self->id, uniformLocation, PyLong_AsLong(value));
        break;
    }
    case GL_UNSIGNED_INT:
    {
        if (!PyLong_Check(value))
        {
            PyErr_Format(PyExc_TypeError, "Expected value to be of type int, got: %s.", Py_TYPE(value)->tp_name);
            return NULL;
        }

        glProgramUniform1ui(self->id, uniformLocation, PyLong_AsUnsignedLong(value));
        break;
    }
    default:
        PyErr_SetString(PyExc_ValueError, "Invalid value type provided.");
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *py_shader_set_uniform_array(PyShader *self, PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {"name", "value", "type", NULL};
    PyObject *result = NULL;

    PyObject *name = NULL;
    Py_buffer dataBuffer = {0};
    GLenum type = GL_FLOAT;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Uy*|I", kwNames, &name, &dataBuffer, &type))
        goto end;

    GLint uniformLocation = get_uniform_location(self, name);
    if (uniformLocation == -1)
        return NULL; // exception already set

    if (!utils_check_buffer_contiguous(&dataBuffer))
        goto end;

    GLsizei count = dataBuffer.len / dataBuffer.itemsize;

    switch (type)
    {
    case GL_FLOAT:
        glProgramUniform1fv(self->id, uniformLocation, count, (const GLfloat *)dataBuffer.buf);
        break;
    case GL_DOUBLE:
        glProgramUniform1dv(self->id, uniformLocation, count, (const GLdouble *)dataBuffer.buf);
        break;
    case GL_INT:
        glProgramUniform1iv(self->id, uniformLocation, count, (const GLint *)dataBuffer.buf);
        break;
    case GL_UNSIGNED_INT:
        glProgramUniform1uiv(self->id, uniformLocation, count, (const GLuint *)dataBuffer.buf);
        break;
    default:
        PyErr_SetString(PyExc_ValueError, "Invalid value type provided.");
        goto end;
    }

    result = Py_NewRef(Py_None);
end:
    if (dataBuffer.buf != NULL)
        PyBuffer_Release(&dataBuffer);

    return result;
}

static PyObject *set_uniform_vec2(PyShader *self, PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {"name", "x", "y", "type", NULL};

    PyObject *name = NULL;
    PyObject *x = NULL;
    PyObject *y = NULL;
    GLenum type = GL_FLOAT;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "UOO|I", kwNames, &name, &x, &y, &type))
        return NULL;

    GLint loc = get_uniform_location(self, name);
    if (loc == -1)
        return NULL;

    switch (type)
    {
    case GL_FLOAT:
        THROW_IF(!(PyFloat_Check(x) && PyFloat_Check(y)), PyExc_TypeError, "all arguments have to be of type float.", NULL);
        glProgramUniform2f(self->id, loc, PyFloat_AS_DOUBLE(x), PyFloat_AS_DOUBLE(y));
        break;
    case GL_DOUBLE:
        THROW_IF(!(PyFloat_Check(x) && PyFloat_Check(y)), PyExc_TypeError, "all arguments have to be of type float.", NULL);
        glProgramUniform2d(self->id, loc, PyFloat_AS_DOUBLE(x), PyFloat_AS_DOUBLE(y));
        break;
    case GL_INT:
        THROW_IF(!(PyLong_Check(x) && PyLong_Check(y)), PyExc_TypeError, "all arguments have to be of type int.", NULL);
        glProgramUniform2i(self->id, loc, PyLong_AsLong(x), PyLong_AsLong(y));
        break;
    case GL_UNSIGNED_INT:
        THROW_IF(!(PyLong_Check(x) && PyLong_Check(y)), PyExc_TypeError, "all arguments have to be of type int.", NULL);
        glProgramUniform2ui(self->id, loc, PyLong_AsLong(x), PyLong_AsLong(y));
        break;
    default:
        PyErr_SetString(PyExc_ValueError, "Invalid uniform type provided.");
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *set_uniform_vec3(PyShader *self, PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {"name", "x", "y", "z", "type", NULL};

    PyObject *name = NULL;
    PyObject *x = NULL;
    PyObject *y = NULL;
    PyObject *z = NULL;
    GLenum type = GL_FLOAT;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "UOOO|I", kwNames, &name, &x, &y, &z, &type))
        return NULL;

    GLint loc = get_uniform_location(self, name);
    if (loc == -1)
        return NULL;

    switch (type)
    {
    case GL_FLOAT:
        THROW_IF(!(PyFloat_Check(x) && PyFloat_Check(y) && PyFloat_Check(z)), PyExc_TypeError, "all arguments have to be of type float.", NULL);
        glProgramUniform3f(self->id, loc, PyFloat_AS_DOUBLE(x), PyFloat_AS_DOUBLE(y), PyFloat_AS_DOUBLE(z));
        break;
    case GL_DOUBLE:
        THROW_IF(!(PyFloat_Check(x) && PyFloat_Check(y) && PyFloat_Check(z)), PyExc_TypeError, "all arguments have to be of type float.", NULL);
        glProgramUniform3d(self->id, loc, PyFloat_AS_DOUBLE(x), PyFloat_AS_DOUBLE(y), PyFloat_AS_DOUBLE(z));
        break;
    case GL_INT:
        THROW_IF(!(PyLong_Check(x) && PyLong_Check(y) && PyLong_Check(z)), PyExc_TypeError, "all arguments have to be of type int.", NULL);
        glProgramUniform3i(self->id, loc, PyLong_AsLong(x), PyLong_AsLong(y), PyLong_AsLong(z));
        break;
    case GL_UNSIGNED_INT:
        THROW_IF(!(PyLong_Check(x) && PyLong_Check(y) && PyLong_Check(z)), PyExc_TypeError, "all arguments have to be of type int.", NULL);
        glProgramUniform3ui(self->id, loc, PyLong_AsLong(x), PyLong_AsLong(y), PyLong_AsLong(z));
        break;
    default:
        PyErr_SetString(PyExc_ValueError, "Invalid uniform type provided.");
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *set_uniform_vec4(PyShader *self, PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {"name", "x", "y", "z", "w", "type", NULL};

    PyObject *name = NULL;
    PyObject *x = NULL;
    PyObject *y = NULL;
    PyObject *z = NULL;
    PyObject *w = NULL;
    GLenum type = GL_FLOAT;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "UOOOO|I", kwNames, &name, &x, &y, &z, &w, &type))
        return NULL;

    GLint loc = get_uniform_location(self, name);
    if (loc == -1)
        return NULL;

    switch (type)
    {
    case GL_FLOAT:
        THROW_IF(!(PyFloat_Check(x) && PyFloat_Check(y) && PyFloat_Check(z) && PyFloat_Check(w)), PyExc_TypeError, "all arguments have to be of type float.", NULL);
        glProgramUniform4f(self->id, loc, PyFloat_AS_DOUBLE(x), PyFloat_AS_DOUBLE(y), PyFloat_AS_DOUBLE(z), PyFloat_AS_DOUBLE(w));
        break;
    case GL_DOUBLE:
        THROW_IF(!(PyFloat_Check(x) && PyFloat_Check(y) && PyFloat_Check(z) && PyFloat_Check(w)), PyExc_TypeError, "all arguments have to be of type float.", NULL);
        glProgramUniform4d(self->id, loc, PyFloat_AS_DOUBLE(x), PyFloat_AS_DOUBLE(y), PyFloat_AS_DOUBLE(z), PyFloat_AS_DOUBLE(w));
        break;
    case GL_INT:
        THROW_IF(!(PyLong_Check(x) && PyLong_Check(y) && PyLong_Check(z) && PyLong_Check(w)), PyExc_TypeError, "all arguments have to be of type int.", NULL);
        glProgramUniform4i(self->id, loc, PyLong_AsLong(x), PyLong_AsLong(y), PyLong_AsLong(z), PyLong_AsLong(w));
        break;
    case GL_UNSIGNED_INT:
        THROW_IF(!(PyLong_Check(x) && PyLong_Check(y) && PyLong_Check(z) && PyLong_Check(w)), PyExc_TypeError, "all arguments have to be of type int.", NULL);
        glProgramUniform4ui(self->id, loc, PyLong_AsLong(x), PyLong_AsLong(y), PyLong_AsLong(z), PyLong_AsLong(w));
        break;
    default:
        PyErr_SetString(PyExc_ValueError, "Invalid uniform type provided.");
        return NULL;
    }

    Py_RETURN_NONE;
}

static bool set_uniform_mat_f(GLuint id, GLint loc, Py_buffer *buf, int len, bool transpose)
{
    SET_UNIFORM_MAT_IMPL(f)
}

static bool set_uniform_mat_d(GLuint id, GLint loc, Py_buffer *buf, int len, bool transpose)
{
    SET_UNIFORM_MAT_IMPL(d)
}

static bool check_mat_buf_valid(Py_buffer *buf, int len, size_t itemSize)
{
    if (!utils_check_buffer_contiguous(buf))
        return false;

    size_t bufSize = len * len * itemSize;
    if (buf->len != bufSize)
    {
        PyErr_SetString(PyExc_ValueError, "Matrix data has invalid byte size. Possibly invalid shape or type provided.");
        return false;
    }

    return true;
}

static bool set_uniform_mat_impl(PyShader *self, PyObject *args, PyObject *kwargs, int len)
{
    static char *kwNames[] = {"name", "value", "type", "transpose", NULL};

    PyObject *name = NULL;
    Py_buffer buf = {0};
    GLenum type = GL_FLOAT;
    bool transpose = false;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Uy*|Ip", kwNames, &name, &buf, &type, &transpose))
        return false;

    GLint loc = get_uniform_location(self, name);
    if (loc == -1)
        return false;

    switch (type)
    {
    case GL_FLOAT:
        if (!check_mat_buf_valid(&buf, len, sizeof(GLfloat)))
            return false;
        set_uniform_mat_f(self->id, loc, &buf, len, transpose);
        return true;
    case GL_DOUBLE:
        if (!check_mat_buf_valid(&buf, len, sizeof(GLdouble)))
            return false;
        set_uniform_mat_d(self->id, loc, &buf, len, transpose);
        return true;
    default:
        PyErr_SetString(PyExc_ValueError, "Invalid uniform type.");
    }

    return false;
}

static PyObject *set_uniform_mat2(PyShader *self, PyObject *args, PyObject *kwargs)
{
    return set_uniform_mat_impl(self, args, kwargs, 2) ? Py_NewRef(Py_None) : NULL;
}

static PyObject *set_uniform_mat3(PyShader *self, PyObject *args, PyObject *kwargs)
{
    return set_uniform_mat_impl(self, args, kwargs, 3) ? Py_NewRef(Py_None) : NULL;
}

static PyObject *set_uniform_mat4(PyShader *self, PyObject *args, PyObject *kwargs)
{
    return set_uniform_mat_impl(self, args, kwargs, 4) ? Py_NewRef(Py_None) : NULL;
}

PyTypeObject pyShaderType = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_new = PyType_GenericNew,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_name = "pygl.shaders.Shader",
    .tp_basicsize = sizeof(PyShader),
    .tp_init = (initproc)py_shader_init,
    .tp_dealloc = (destructor)py_shader_dealloc,
    .tp_members = (PyMemberDef[]){
        {"id", T_UINT, offsetof(PyShader, id), 0, NULL},
        {"resources", T_OBJECT_EX, offsetof(PyShader, resources), READONLY, NULL},
        {0}},
    .tp_methods = (PyMethodDef[]){
        {"delete", (PyCFunction)py_shader_delete, METH_NOARGS, NULL},
        {"use", (PyCFunction)py_shader_use, METH_NOARGS, NULL},
        {"get_resource_location", (PyCFunction)py_shader_get_resource_location, METH_O, NULL},
        {"validate", (PyCFunction)py_shader_validate, METH_NOARGS, NULL},
        {"set_uniform_block_binding", (PyCFunction)py_shader_set_uniform_block_binding, METH_VARARGS, NULL},
        {"get_uniform_block_location", (PyCFunction)py_shader_get_uniform_block_location, METH_O, NULL},
        {"set_uniform", (PyCFunction)py_shader_set_uniform, METH_VARARGS | METH_KEYWORDS, NULL},
        {"set_uniform_array", (PyCFunction)py_shader_set_uniform_array, METH_VARARGS | METH_KEYWORDS, NULL},
        {"set_uniform_vec2", (PyCFunction)set_uniform_vec2, METH_VARARGS | METH_KEYWORDS, NULL},
        {"set_uniform_vec3", (PyCFunction)set_uniform_vec3, METH_VARARGS | METH_KEYWORDS, NULL},
        {"set_uniform_vec4", (PyCFunction)set_uniform_vec4, METH_VARARGS | METH_KEYWORDS, NULL},
        {"set_uniform_mat2", (PyCFunction)set_uniform_mat2, METH_VARARGS | METH_KEYWORDS, NULL},
        {"set_uniform_mat3", (PyCFunction)set_uniform_mat3, METH_VARARGS | METH_KEYWORDS, NULL},
        {"set_uniform_mat4", (PyCFunction)set_uniform_mat4, METH_VARARGS | METH_KEYWORDS, NULL},
        {0},
    },
};
