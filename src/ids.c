#include <uEye.h>
#include "ids.h"

PyObject * IDSError;

PyObject * ids_number_cameras(PyObject * self)
{
    int num_cams, ret;

    ret = is_GetNumberOfCameras(&num_cams);
    if (ret != IS_SUCCESS)
    {
        PyErr_Format(IDSError, "uEye SDK error %d", ret);
        return NULL;
    }
    return Py_BuildValue("i", num_cams);
}

PyMethodDef idsMethods[] =
{
    {"num_cams", (PyCFunction)ids_number_cameras, METH_NOARGS,
     "Determines the total number of available cameras\n"
    },
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

    /* IDS Exceptions */
    IDSError = PyErr_NewExceptionWithDoc("ids_core.IDSError",
            "Base class for exceptions caused by an error with the IDS camera or libraries.",
            NULL, NULL);
    Py_INCREF(IDSError);
    PyModule_AddObject(m, "IDSError", IDSError);
   
    /* IDS Objects */
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

    /* IDS Exceptions */
    IDSError = PyErr_NewExceptionWithDoc("ids.IDSError",
            "Base class for exceptions caused by an error with the IDS camera or libraries.",
            NULL, NULL);
    Py_INCREF(IDSError);
    PyModule_AddObject(m, "IDSError", IDSError);
   
    /* IDS Objects */
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
