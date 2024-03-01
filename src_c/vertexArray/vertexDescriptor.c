#include "vertexDescriptor.h"
#include <structmember.h>

static int py_vaolayout_init(PyVertexDescriptor* self, PyObject* args, PyObject* kwargs)
{
    static char* kwNames[] = { "attrib_index", "type", "count", "rows", "is_normalized", NULL };
    if (!PyArg_ParseTupleAndKeywords(
            args,
            kwargs,
            "IIi|ip",
            kwNames,
            &self->attribIndex, &self->type, &self->count, &self->rows, &self->isNormalized))
        return -1;

    return 0;
}

PyTypeObject pyVertexDescriptorType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_new = PyType_GenericNew,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_name = "pygl.vertex_array.VertexDescriptor",
    .tp_basicsize = sizeof(PyVertexDescriptor),
    .tp_init = (initproc)py_vaolayout_init,
    .tp_members = (PyMemberDef[]) {
        {"attrib_index", T_UINT, offsetof(PyVertexDescriptor, attribIndex), 0, NULL},
        {"type", T_UINT, offsetof(PyVertexDescriptor, type), 0, NULL},
        {"count", T_INT, offsetof(PyVertexDescriptor, count), 0, NULL},
        {"rows", T_INT, offsetof(PyVertexDescriptor, rows), 0, NULL},
        {"is_normalized", T_BOOL, offsetof(PyVertexDescriptor, isNormalized), 0, NULL},
        {0},
    },
};
