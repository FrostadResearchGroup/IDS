#include <uEye.h>
#include "ids.h"

PyObject * IDSError;

/**
  * General function to print errors that may occur whilst using the SDK
  */  
void print_error(Camera * self)
{
    int errorCode;
    char * message;

    int returnCode = is_GetError(self->handle, &errorCode, &message);
    if (returnCode != IS_SUCCESS)
    {
        message = "Could not obtain error";
    }
    PyErr_Format(IDSError, "uEye SDK error %d %s", returnCode, message);
}

/**
  * Returns the number of connected cameras 
  */
PyObject * ids_number_cameras(PyObject * self)
{
    int num_cams, returnCode;

    returnCode = is_GetNumberOfCameras(&num_cams);
    if (returnCode != IS_SUCCESS)
    {
        PyErr_Format(IDSError, "uEye SDK error %d", returnCode);
        return NULL;
    }
    return Py_BuildValue("i", num_cams);
}

/**
  * Returns information about all the connected cameras
  */  
PyObject * ids_all_cameras_info(PyObject * self)
{
    int num_cams, returnCode, i;
    UEYE_CAMERA_LIST * cameras;
    PyObject * list = PyList_New(0);
    PyObject * camera_info;
    PyObject * camera_id;
    PyObject * device_id;
    PyObject * sensor_id;
    PyObject * in_use;
    PyObject * serial_number;
    PyObject * model;
    PyObject * status;

    returnCode = is_GetNumberOfCameras(&num_cams);
    if (returnCode != IS_SUCCESS)
    {
        PyErr_Format(IDSError, "uEye SDK error %d\n", returnCode);
        return NULL;
    }


    cameras = (UEYE_CAMERA_LIST *) malloc(sizeof(DWORD) + num_cams * sizeof(UEYE_CAMERA_INFO));
    cameras->dwCount = num_cams;

    if (!num_cams)
    {
        return list;
    }

    returnCode = is_GetCameraList(cameras);
    if (returnCode != IS_SUCCESS)
    {
        Py_DECREF(list);
        PyErr_Format(IDSError, "uEye SDK error %d\n", returnCode);
        return NULL;
    }

    for (i = 0; i < cameras->dwCount; i++) 
    {
        camera_info = PyDict_New();

        camera_id = Py_BuildValue("I", cameras->uci[i].dwCameraID);
        device_id = Py_BuildValue("I", cameras->uci[i].dwDeviceID);
        sensor_id = Py_BuildValue("I", cameras->uci[i].dwSensorID);
        in_use = Py_BuildValue("O", cameras->uci[i].dwInUse ? Py_True : Py_False);
        serial_number = Py_BuildValue("s", cameras->uci[i].SerNo);
        model = Py_BuildValue("s", cameras->uci[i].Model);
        status = Py_BuildValue("I", cameras->uci[i].dwStatus);

        PyDict_SetItemString(camera_info, "camera_id", camera_id);
        PyDict_SetItemString(camera_info, "device_id", device_id);
        PyDict_SetItemString(camera_info, "sensor_id", sensor_id);
        PyDict_SetItemString(camera_info, "in_use", in_use);
        PyDict_SetItemString(camera_info, "serial_number", serial_number);
        PyDict_SetItemString(camera_info, "model", model);
        PyDict_SetItemString(camera_info, "status", status);

        PyList_Append(list, camera_info);

        Py_DECREF(camera_info);
    }
    Py_DECREF(camera_id);
    Py_DECREF(device_id);
    Py_DECREF(sensor_id);
    Py_DECREF(in_use);
    Py_DECREF(serial_number);
    Py_DECREF(model);
    Py_DECREF(status);

    free(cameras);
    return list;
}

PyMethodDef idsMethods[] =
{
    {"num_cams", (PyCFunction)ids_number_cameras, METH_NOARGS,
     "Determines the total number of available cameras\n"
    },
    {"all_cams_info", (PyCFunction)ids_all_cameras_info, METH_NOARGS,
     "Returns a list of all camera information"
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
