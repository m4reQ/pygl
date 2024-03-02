#include "utility.h"

bool utils_check_buffer_contiguous(const Py_buffer* buf)
{
    if (!PyBuffer_IsContiguous(buf, 'C'))
    {
        PyErr_SetString(PyExc_ValueError, "Data buffer has to be C-contiguous. For more informations go to https://github.com/m4reQ/pygl?tab=readme-ov-file#buffer-protocol-usage.");
        return false;
    }

    return true;
}
