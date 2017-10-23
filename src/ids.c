#include <uEye.h>
#include "ids.h"

PyMethodDef idsMethods[] =
{
    {NULL, NULL, 0, NULL} /* sentinel */
};

#if PY_MAJOR_VERSION >= 3
/*
 * This is only needed for Python3
 * IDS module initialization
 * Based on https://docs.python.org/3/extending/extending.html#the-module-s-method-table-and-initialization-function
 */
static struct PyModuleDef idsModule = {
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
    PyObject * m;

    ids_CameraType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&ids_CameraType) < 0)
        return NULL;

    m = PyModule_Create(&idsModule);
    if (m == NULL)
    {
        return NULL;
    }

    Py_INCREF(&ids_CameraType);
    PyModule_AddObject(m, "Camera", (PyObject *)(&ids_CameraType));
    return m;
}
#else
PyMODINIT_FUNC initids(void)
{
    PyObject* m;

    ids_CameraType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&ids_CameraType) < 0)
        return NULL;

    m = Py_InitModule("ids", idsMethods);

    Py_INCREF(&ids_CameraType);
    PyModule_AddObject(m, "Camera", (PyObject *)(&ids_CameraType));
}
#endif

int main(int argc, char *argv[])
{
#if PY_MAJOR_VERSION >= 3
    wchar_t name[128];
    mbstowcs(name, argv[0], 128);
#else
    char name[128];
    strncpy(name, argv[0], 128);
#endif    
    /* Pass argv[0] to the Pythin interpreter */
    Py_SetProgramName(name);

    /* Initialize the Python interpreter */
    Py_Initialize();

    /* Add a static module */
#if PY_MAJOR_VERSION >= 3
    PyInit_ids();
#else
    initids();
#endif
}