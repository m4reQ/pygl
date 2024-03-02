#include "vertexArray.h"
#include "vertexInput.h"
#include "vertexDescriptor.h"
#include "../buffers/bufferBase.h"
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

static bool set_attrib_format(GLuint array, PyVertexDescriptor* desc, GLuint* offset)
{
    switch (desc->type)
    {
    case GL_DOUBLE:
        glVertexArrayAttribLFormat(
            array,
            desc->attribIndex,
            desc->count,
            desc->type,
            *offset);
        break;
    case GL_FLOAT:
    case GL_HALF_FLOAT:
        glVertexArrayAttribFormat(
            array,
            desc->attribIndex,
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
        glVertexArrayAttribIFormat(
            array,
            desc->attribIndex,
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

static bool add_vertex_input(GLuint array, GLuint index, PyVertexInput* input, GLuint* attribOffset)
{
    glVertexArrayVertexBuffer(array, index, input->bufferId, input->offset, input->stride);
    glVertexArrayBindingDivisor(array, index, input->divisor);

    int nDescriptors = PyList_GET_SIZE(input->descriptors);
    for (int i = 0; i < nDescriptors; i++)
    {
        PyVertexDescriptor* descriptor = (PyVertexDescriptor*)PyList_GET_ITEM(input->descriptors, i);
        ThrowIf(
            !PyObject_IsInstance((PyObject*)descriptor, (PyObject*)&pyVertexDescriptorType),
            PyExc_TypeError,
            "All vertex descriptors objects has to be of type pygl.vertex_array.VertexDescriptor",
            false);

        glEnableVertexArrayAttrib(array, descriptor->attribIndex);
        glVertexArrayAttribBinding(array, descriptor->attribIndex, index);

        for (int row = 0; row < descriptor->rows; i++)
            if (!set_attrib_format(array, descriptor, attribOffset))
                return false;
    }

    return true;
}

static int py_vertex_array_init(PyVertexArray* self, PyObject* args, PyObject* kwargs)
{
    static char* kwNames[] = {"layout", "element_buffer", NULL};

    PyObject* vertexInputs = NULL;
    PyBufferBase* elementBuffer = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!|O", kwNames, &PyList_Type, &vertexInputs, &elementBuffer))
        return -1;

    if (elementBuffer)
        ThrowIf(
            !is_buffer((PyObject*)elementBuffer),
            PyExc_TypeError,
            "Element buffer has to be of type pygl.buffers.Buffer or pygl.buffers.MappedBuffer",
            -1);

    glCreateVertexArrays(1, &self->id);

    int nInputs = PyList_GET_SIZE(vertexInputs);
    for (int i = 0; i < nInputs; i++)
    {
        GLuint attribOffset = 0;
        PyVertexInput* input = (PyVertexInput*)PyList_GET_ITEM(vertexInputs, i);

        ThrowIf(
            !PyList_Check(input->descriptors),
            PyExc_TypeError,
            "layout must be a list of pygl.vertex_array.VertexInput objects.",
            -1);

        if (!add_vertex_input(self->id, (GLuint)i, input, &attribOffset))
            return -1;
    }

    if (elementBuffer)
        glVertexArrayElementBuffer(self->id, elementBuffer->id);

    return 0;
}

static PyObject* py_vertex_array_delete(PyVertexArray* self, PyObject* Py_UNUSED(args))
{
    glDeleteVertexArrays(1, &self->id);
    self->id = 0;

    Py_RETURN_NONE;
}

static PyObject* py_vertex_array_bind(PyVertexArray* self, PyObject* Py_UNUSED(args))
{
    glBindVertexArray(self->id);
    Py_RETURN_NONE;
}

static void py_vertex_array_dealloc(PyVertexArray* self)
{
    glDeleteVertexArrays(1, &self->id);
    Py_TYPE(self)->tp_free(self);
}

static PyMethodDef pyVertexArrayMethods[] = {
    {"delete", (PyCFunction)py_vertex_array_delete, METH_NOARGS, NULL},
    {"bind", (PyCFunction)py_vertex_array_bind, METH_NOARGS, NULL},
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
