#include "vertexInput.h"
#include <structmember.h>
#include "../buffers/buffer.h"
#include "../buffers/mappedBuffer.h"
#include "../utility.h"
#include "../buffers/bufferBase.h"
#include "vertexDescriptor.h"

static int py_vertexinput_init(PyVertexInput* self, PyObject* args, PyObject* kwargs)
{
    static char* kwNames[] = {"buffer", "stride", "descriptors", "offset", "divisor", NULL};

    PyObject* buffer = NULL; // not typechecked!
    PyObject* descriptors = NULL;
    if (!PyArg_ParseTupleAndKeywords(
            args, kwargs, "OiO!|LI", kwNames,
            &buffer, &self->stride, &PyList_Type, &descriptors,
            &self->offset, &self->divisor))
        return -1;

    ThrowIf(
        !is_buffer(buffer),
        PyExc_TypeError,
        "Buffer must be either of pygl.buffers.Buffer or pygl.buffers.MappedBuffer type.",
        -1);

    self->bufferId = ((PyBufferBase*)buffer)->id;

    // check types of every element in `descriptors` here to avoid later errors
    Py_ssize_t nDesc = PyList_GET_SIZE(descriptors);
    for (Py_ssize_t i = 0; i < nDesc; i++)
        ThrowIf(
            !PyObject_IsInstance(PyList_GET_ITEM(descriptors, i), &pyVertexDescriptorType),
            PyExc_TypeError,
            "Every element in descriptors list has to be of type pygl.VertexDescriptor",
            -1);

    self->descriptors = Py_NewRef(descriptors);

    return 0;
}

static void py_vertexinput_dealloc(PyVertexInput* self)
{
    Py_CLEAR(self->descriptors);
    Py_TYPE(self)->tp_free(self);
}

PyTypeObject pyVertexInputType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_name = "pygl.vertex_array.VertexInput",
    .tp_init = py_vertexinput_init,
    .tp_dealloc = py_vertexinput_dealloc,
    .tp_basicsize = sizeof(PyVertexInput),
    .tp_members = (PyMemberDef[]) {
        {"buffer_id", T_UINT, offsetof(PyVertexInput, bufferId), 0, NULL},
        {"stride", T_INT, offsetof(PyVertexInput, stride), 0, NULL},
        {"descriptors", T_OBJECT_EX, offsetof(PyVertexInput, descriptors), 0, NULL},
        {"offset", T_LONGLONG, offsetof(PyVertexInput, offset), 0, NULL},
        {"divisor", T_UINT, offsetof(PyVertexInput, divisor), 0, NULL},
        {0},
    },
};
