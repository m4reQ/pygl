#include <stdbool.h>
#include <stdio.h>
#include "shader.h"
#include "shaderStage.h"
#include "../utility.h"

#define READ_FILE_CHUNK_SIZE 1024 * 64 // 64 KiB

typedef struct
{
    PyObject_HEAD
    GLuint id;
    PyObject* resources;
} PyShader;

typedef struct
{
    GLint location;
    GLint nameLength;
} ResourceProto;

static GLint get_uniform_location(PyShader* shader, PyObject* name)
{
    PyObject* locationObj = PyDict_GetItem(shader->resources, name);
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
    char* infoLog = PyMem_Malloc(bufSize);
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
    char* infoLog = PyMem_Malloc(bufferSize);
    glGetProgramInfoLog(shader, bufferSize, NULL, infoLog);

    PyErr_Format(PyExc_RuntimeError, "Shader link failure:\n%s", infoLog);
    PyMem_Free(infoLog);
    return -1;
}

static bool read_whole_file(FILE* file, char** outData, size_t* outSize)
{
    char* buffer = NULL;
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

static GLuint create_stage(GLenum type, const char* filepath)
{
    FILE* sourceFile = NULL;
    if (fopen_s(&sourceFile, filepath, "r"))
    {
        PyErr_Format(PyExc_FileNotFoundError, "Couldn't open shader stage source file: %s.", filepath);
        return -1;
    }

    char* source;
    size_t sourceLen;
    ThrowIf(
        !read_whole_file(sourceFile, &source, &sourceLen),
        PyExc_RuntimeError,
        "Couldn't read shader stage source file.",
        -1);

    GLuint stage = glCreateShader(type);
    glShaderSource(stage, 1, &source, &sourceLen);

    glCompileShader(stage);

    PyMem_Free(source);

    if (!check_compile_success(stage))
        return -1;

    return stage;
}

static void retrieve_interface(GLuint shader, GLenum interface, PyObject* storage)
{
    GLint resourceCount = 0;
    glGetProgramInterfaceiv(shader, interface, GL_ACTIVE_RESOURCES, &resourceCount);

    for (GLint i = 0; i < resourceCount; i++)
    {
        static const GLenum properties[] = { GL_LOCATION, GL_NAME_LENGTH };

        ResourceProto proto;
        glGetProgramResourceiv(shader, interface, i, _countof(properties), properties, sizeof(ResourceProto), NULL, &proto);

        // skip members of buffer uniform blocks
        if (proto.location == -1)
            continue;

        const size_t bufSize = sizeof(char) * (proto.nameLength + 1);
        char* name = PyMem_Malloc(bufSize);
        glGetProgramResourceName(shader, interface, i, bufSize, NULL, name);
        name[bufSize - 1] = '\0';

        // normalize array names (array[0] -> array)
        char* bracket = strchr(name, '[');
        if (bracket)
            *bracket = '\0';

        PyDict_SetItemString(storage, name, PyLong_FromLong(proto.location));
    }
}

static int py_shader_init(PyShader* self, PyObject* args, PyObject* Py_UNUSED(kwargs))
{
    PyObject* stages;
    if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &stages))
        return -1;

    self->id = glCreateProgram();
    self->resources = PyDict_New();

    Py_ssize_t nStages = PyList_GET_SIZE(stages);
    GLuint* attachedStages = PyMem_Malloc(sizeof(GLuint) * nStages);

    for (Py_ssize_t i = 0; i < nStages; i++)
    {
        PyShaderStage* _stage = (PyShaderStage*)PyList_GET_ITEM(stages, i);
        ThrowIf(
            !PyObject_IsInstance(_stage, &pyShaderStageType),
            PyExc_TypeError,
            "Second argument must be a list of pygl.shaders.ShaderStage objects",
            -1);

        GLuint stage = create_stage(_stage->type, PyUnicode_AsUTF8(_stage->pyFilepath));
        if (stage == -1)
            // exception already set by create_stage
            return stage;

        glAttachShader(self->id, stage);
        attachedStages[i] = stage;
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

static PyObject* py_shader_delete(PyShader* self, PyObject* Py_UNUSED(args))
{
    glDeleteProgram(self->id);
    self->id = 0;

    Py_RETURN_NONE;
}

static void py_shader_dealloc(PyShader* self)
{
    py_shader_delete(self, NULL);
    Py_CLEAR(self->resources);
    Py_TYPE(self)->tp_free(self);
}

static PyObject* py_shader_use(PyShader* self, PyObject* Py_UNUSED(args))
{
    glUseProgram(self->id);
    Py_RETURN_NONE;
}

static PyObject* py_shader_get_resource_location(PyShader* self, PyObject* name)
{
    ThrowIf(
        !PyUnicode_Check(name),
        PyExc_TypeError,
        "Name has to be of type str.",
        NULL);

    PyObject* location = PyDict_GetItem(self->resources, name);
    if (!location)
        PyErr_Format(PyExc_RuntimeError, "Couldn't find shader resource with name: %U.", name);

    return location;
}

static PyObject* py_shader_validate(PyShader* self, PyObject* Py_UNUSED(args))
{
    glValidateProgram(self->id);

    GLint success = GL_FALSE;
    glGetProgramiv(self->id, GL_VALIDATE_STATUS, &success);
    if (!success)
    {
        GLint logLength = 0;
        glGetProgramiv(self->id, GL_INFO_LOG_LENGTH, &logLength);

        char* buffer = PyMem_Malloc(sizeof(char) * logLength);
        glGetProgramInfoLog(self->id, sizeof(char) * logLength, NULL, buffer);

        PyErr_Format(PyExc_RuntimeError, "Shader %d validation error:\n%s.", self->id, buffer);
        PyMem_Free(buffer);

        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject* py_shader_get_uniform_block_location(PyShader* self, PyObject* name)
{
    ThrowIf(
        !PyUnicode_Check(name),
        PyExc_TypeError,
        "name has to be of type str.",
        NULL);

    GLuint index = glGetUniformBlockIndex(self->id, PyUnicode_AsUTF8(name));
    return PyLong_FromUnsignedLong(index);
}

static PyObject* py_shader_set_uniform_block_binding(PyShader* self, PyObject* args)
{
    char* name = NULL;
    GLuint binding = 0;
    if (!PyArg_ParseTuple(args, "sI", &name, &binding))
        return NULL;

    glUniformBlockBinding(self->id, glGetUniformBlockIndex(self->id, name), binding);
    Py_RETURN_NONE;
}

static PyObject* py_shader_set_uniform(PyShader* self, PyObject* args)
{
    PyObject* name = NULL;
    PyObject* value = NULL;
    if (!PyArg_ParseTuple(args, "UO", &name, &value))
        return NULL;

    GLint uniformLocation = get_uniform_location(self, name);
    if (uniformLocation == -1)
        return NULL; // exception already set

    if (PyLong_Check(value))
    {
        int overflow = 0;
        int _value = PyLong_AsLongAndOverflow(value, &overflow);
        if (overflow == -1)
            glProgramUniform1i(self->id, uniformLocation, _value);
        else
            glProgramUniform1ui(self->id, uniformLocation, (GLuint)_value);
    }
    else if (PyFloat_Check(value))
    {
        // find way to allow setting double type uniforms (maybe change API to accept explicit value type?)
        glProgramUniform1f(self->id, uniformLocation, (GLfloat)PyFloat_AS_DOUBLE(value));
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "Expected value to be of type int or float.");
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject* py_shader_set_uniform_array(PyShader* self, PyObject* args)
{
    PyObject* result = NULL;

    PyObject* name = NULL;
    Py_buffer dataBuffer = {0};
    GLenum type = 0;

    if (!PyArg_ParseTuple(args, "Uy*I", &name, &dataBuffer, &type))
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
        glProgramUniform1fv(self->id, uniformLocation, count, (const GLfloat*)dataBuffer.buf);
        break;
    case GL_DOUBLE:
        glProgramUniform1dv(self->id, uniformLocation, count, (const GLdouble*)dataBuffer.buf);
        break;
    case GL_INT:
        glProgramUniform1iv(self->id, uniformLocation, count, (const GLint*)dataBuffer.buf);
        break;
    case GL_UNSIGNED_INT:
        glProgramUniform1uiv(self->id, uniformLocation, count, (const GLuint*)dataBuffer.buf);
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

static PyMethodDef pyShaderMethods[] = {
    {"delete", py_shader_delete, METH_NOARGS, NULL},
    {"use", py_shader_use, METH_NOARGS, NULL},
    {"get_resource_location", py_shader_get_resource_location, METH_O, NULL},
    {"validate", py_shader_validate, METH_NOARGS, NULL},
    {"set_uniform_block_binding", py_shader_set_uniform_block_binding, METH_VARARGS, NULL},
    {"get_uniform_block_location", py_shader_get_uniform_block_location, METH_O, NULL},
    {"set_uniform", py_shader_set_uniform, METH_VARARGS, NULL},
    {"set_uniform_array", py_shader_set_uniform_array, METH_VARARGS, NULL},
    // TODO set_uniform_matrix and set_uniform_vector
    {0},
};

static PyMemberDef pyShaderMembers[] = {
    {"id", T_UINT, offsetof(PyShader, id), 0, NULL},
    {"resources", T_OBJECT_EX, offsetof(PyShader, resources), READONLY, NULL},
    {0},
};

PyTypeObject pyShaderType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_new = PyType_GenericNew,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_name = "pygl.shaders.Shader",
    .tp_basicsize = sizeof(PyShader),
    .tp_init = py_shader_init,
    .tp_dealloc = py_shader_dealloc,
    .tp_methods = pyShaderMethods,
    .tp_members = pyShaderMembers,
};
