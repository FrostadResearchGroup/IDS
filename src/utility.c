#include <uEye.h>
#include "ids.h"

char * get_as_string(PyObject * value)
{
#ifdef IS_PY3
    return PyUnicode_AsUTF8(value);
#else
    return PyString_AsString(value);
#endif
}

int check_is_string(PyObject * value)
{
#ifdef IS_PY3
    return PyUnicode_Check(value);
#else
    return PyString_Check(value);
#endif
}
