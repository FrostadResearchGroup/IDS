#include <Python.h>
#include <uEye.h>
#include "core.h"

#if PY_MAJOR_VERSION >= 3
/*
 * This is only needed for Python3
 * IDS module initialization
 * Based on https://docs.python.org/3/extending/extending.html#the-module-s-method-table-and-initialization-function
 */
static struct PyModuleDef idsModule = 
{
    PyModuleDef_HEAD_INIT,
    "ids", /* name of module */
    NULL,  /* module documentation */
    -1,    /* size of perinterpreter state of the module,
              or -1 if the module keeps state in global variables. */
    idsMethods
};
#endif

#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC PyInit_ids(void)
{
    return PyModule_Create(&idsModule);
}
#else
PyMODINIT_FUNC initids(void)
{
    (void) Py_InitModule("ids", idsMethods);
}
#endif

