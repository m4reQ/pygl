#include "vertexArray.h"
#include "vertexInput.h"
#include "vertexDescriptor.h"
#include "../utility.h"
#include <stdbool.h>
#include <assert.h>

static GLuint get_gl_type_size(GLenum type)
{
    switch (type)
    {
    case GL_DOUBLE:
        return sizeof(GLdouble);
    case GL_FLOAT:
        return sizeof(GLfloat);
    case GL_HALF_FLOAT:
        return sizeof(GLhalf);
    case GL_INT:
        return sizeof(GLint);
    case GL_UNSIGNED_INT:
        return sizeof(GLuint);
    case GL_SHORT:
        return sizeof(GLshort);
    case GL_UNSIGNED_SHORT:
        return sizeof(GLushort);
    case GL_BYTE:
        return sizeof(GLbyte);
    case GL_UNSIGNED_BYTE:
        return sizeof(GLubyte);
    }

    assert(0 && "Unreachable at vertexArray::get_gl_type_size");
    return 0;
}

static bool set_attrib_format(GLuint array, PyVertexDescriptor *desc, GLuint *offset, GLuint rowOffset)
{
    switch (desc->type)
    {
    case GL_DOUBLE:
        glVertexArrayAttribLFormat(
            array,
            desc->attribIndex + rowOffset,
            desc->count,
            desc->type,
            *offset);
        break;
    case GL_FLOAT:
    case GL_HALF_FLOAT:
        glVertexArrayAttribFormat(
            array,
            desc->attribIndex + rowOffset,
            desc->count,
            desc->type,
            (GLboolean)desc->isNormalized,
            *offset);
        break;
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        // allow to use normalized int attribs
        if (desc->isNormalized)
            glVertexArrayAttribFormat(
                array,
                desc->attribIndex + rowOffset,
                desc->count,
                desc->type,
                (GLboolean)desc->isNormalized,
                *offset);
        else
            glVertexArrayAttribIFormat(
                array,
                desc->attribIndex + rowOffset,
                desc->count,
                desc->type,
                *offset);
        break;
    default:
        PyErr_Format(PyExc_ValueError, "Invalid attribute type: %d.", desc->type);
        return false;
    }

    *offset += get_gl_type_size(desc->type) * desc->count;
    return true;
}

static bool add_vertex_input(GLuint array, GLuint index, PyVertexInput *input)
{
    GLuint attribOffset = 0;

    glVertexArrayVertexBuffer(array, index, input->bufferId, input->offset, input->stride);
    glVertexArrayBindingDivisor(array, index, input->divisor);

    int nDescriptors = PyList_GET_SIZE(input->descriptors);
    for (int i = 0; i < nDescriptors; i++)
    {
        PyVertexDescriptor *descriptor = (PyVertexDescriptor *)PyList_GET_ITEM(input->descriptors, i);
        THROW_IF(
            !PyObject_IsInstance((PyObject *)descriptor, (PyObject *)&pyVertexDescriptorType),
            PyExc_TypeError,
            "All vertex descriptors objects has to be of type pygl.vertex_array.VertexDescriptor",
            false);

        for (int row = 0; row < descriptor->rows; row++)
        {
            glEnableVertexArrayAttrib(array, descriptor->attribIndex + row);
            glVertexArrayAttribBinding(array, descriptor->attribIndex + row, index);

            if (!set_attrib_format(array, descriptor, &attribOffset, row))
                return false;
        }
    }

    return true;
}

static int py_vertex_array_init(PyVertexArray *self, PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {"layout", "element_buffer", NULL};

    PyObject *vertexInputs = NULL;
    PyBuffer *elementBuffer = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!|O", kwNames, &PyList_Type, &vertexInputs, &elementBuffer))
        return -1;

    glCreateVertexArrays(1, &self->id);

    if (elementBuffer)
    {
        THROW_IF(
            !PyObject_IsInstance((PyObject *)elementBuffer, (PyObject *)&pyBufferType),
            PyExc_TypeError,
            "Element buffer has to be of type pygl.buffers.Buffer or pygl.buffers.MappedBuffer",
            -1);

        glVertexArrayElementBuffer(self->id, elementBuffer->id);
    }

    int nInputs = PyList_GET_SIZE(vertexInputs);
    for (int i = 0; i < nInputs; i++)
    {
        PyVertexInput *input = (PyVertexInput *)PyList_GET_ITEM(vertexInputs, i);

        THROW_IF(
            !PyList_Check(input->descriptors),
            PyExc_TypeError,
            "layout must be a list of pygl.vertex_array.VertexInput objects.",
            -1);

        if (!add_vertex_input(self->id, (GLuint)i, input))
            return -1;
    }

    return 0;
}

static PyObject *py_vertex_array_delete(PyVertexArray *self, PyObject *Py_UNUSED(args))
{
    glDeleteVertexArrays(1, &self->id);
    self->id = 0;

    Py_RETURN_NONE;
}

static PyObject *py_vertex_array_bind(PyVertexArray *self, PyObject *Py_UNUSED(args))
{
    glBindVertexArray(self->id);
    Py_RETURN_NONE;
}

static void py_vertex_array_dealloc(PyVertexArray *self)
{
    glDeleteVertexArrays(1, &self->id);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *bind_vertex_buffer(PyVertexArray *self, PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {"buffer", "index", "stride", "offset", "divisor", NULL};

    PyBuffer *buffer;
    GLuint index;
    GLsizei stride;
    GLintptr offset = 0;
    GLuint divisor = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!Ii|nI", kwNames, &pyBufferType, &buffer, &index, &stride, &offset, &divisor))
        return NULL;

    glVertexArrayVertexBuffer(self->id, index, buffer->id, offset, stride);
    glVertexArrayBindingDivisor(self->id, index, divisor);

    Py_RETURN_NONE;
}

static PyObject *bind_index_buffer(PyVertexArray *self, PyBuffer *buffer)
{
    if (!PyObject_IsInstance((PyObject *)buffer, (PyObject *)&pyBufferType))
    {
        PyErr_Format(PyExc_TypeError, "Expected argument to be of type pygl.buffers.Buffer, got: %s.", Py_TYPE(buffer)->tp_name);
        return NULL;
    }

    glVertexArrayElementBuffer(self->id, buffer->id);

    Py_RETURN_NONE;
}

static PyMethodDef pyVertexArrayMethods[] = {
    {"delete", (PyCFunction)py_vertex_array_delete, METH_NOARGS, NULL},
    {"bind", (PyCFunction)py_vertex_array_bind, METH_NOARGS, NULL},
    {"bind_vertex_buffer", (PyCFunction)bind_vertex_buffer, METH_VARARGS | METH_KEYWORDS, NULL},
    {"bind_index_buffer", (PyCFunction)bind_index_buffer, METH_O, NULL},
    {0},
};

static PyMemberDef pyVertexArrayMembers[] = {
    {"id", T_UINT, offsetof(PyVertexArray, id), 0, NULL},
    {0},
};

PyTypeObject pyVertexArrayType = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_new = PyType_GenericNew,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_name = "pygl.vertex_array.VertexArray",
    .tp_basicsize = sizeof(PyVertexArray),
    .tp_init = (initproc)py_vertex_array_init,
    .tp_dealloc = (destructor)py_vertex_array_dealloc,
    .tp_members = pyVertexArrayMembers,
    .tp_methods = pyVertexArrayMethods,
};
