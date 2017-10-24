#include <uEye.h>
#include "ids.h"
#include "structmember.h"

/*
 * Initialize the attributes of the newly created Camera object to 0
 */
PyObject* camera_new
(
    PyTypeObject * type,
    PyObject * args,
    PyObject * kwds
)
{
    Camera *self;

    self = (Camera *)type->tp_alloc(type, 0);

    if (self != NULL)
    {
        self->handle       = 0;
        self->width        = 0;
        self->height       = 0;
        self->bitdepth     = 0;
        self->color        = 0;
        self->autofeatures = 0;
        self->status       = (int)NOT_READY;
    }
    return(PyObject *)self;
}

/*
 * Deallocates all the allocated memory of the Camera
 */
void camera_dealloc(Camera* self)
{
    Py_TYPE(self)->tp_free((PyObject *)self);
}

/*
 * Obtain the hard coded data in the non-volatile camera memory
 * @return A Python Dictionary that contains key-value pairs of this data
 * @note The keys of the dictionary are as follow:
 *      serial_num   : The serial number of the camera
 *      manufacturer : The manufacturer of the camera
 *      date         : The manufacturing date of the camera
 *      select       : Customizable id of the camera
 *      type         : Type of the camera
 */
PyObject * camera_camera_info(Camera * self)
{
    CAMINFO cam_info;

    int returnCode = is_GetCameraInfo(self->handle, &cam_info);
    if (returnCode != IS_SUCCESS)
    {
        // handle error
        return NULL;
    }

    PyObject * dict = PyDict_New();
    
    PyObject * serial_num = PyBytes_FromString(cam_info.SerNo);
    PyObject * manufacturer = PyBytes_FromString(cam_info.ID);
    PyObject * hw_version = PyBytes_FromString(cam_info.Version);
    PyObject * manufacture_date = PyBytes_FromString(cam_info.Date);
    PyObject * select = Py_BuildValue('b', cam_info.Select);
    PyObject * type;

    switch(cam_info.Type)
    {
        case IS_CAMERA_TYPE_UEYE_USB_SE:
            type = PyBytes_FromString("USB uEye SE");
            break;
        case IS_CAMERA_TYPE_UEYE_USB_LE:
            type = PyBytes_FromString("USB uEye LE");
            break;
        case IS_CAMERA_TYPE_UEYE_USB_ML:
            type = PyBytes_FromString("USB uEye ML");
            break;
        case IS_CAMERA_TYPE_UEYE_USB3_CP:
            type = PyBytes_FromString("USB 3 uEye CP");
            break;
        case IS_CAMERA_TYPE_UEYE_USB3_LE:
            type = PyBytes_FromString("USB 3 uEye LE");
            break;
        case IS_CAMERA_TYPE_UEYE_USB3_ML:
            type = PyBytes_FromString("USB 3 uEye ML");
            break;
        case IS_CAMERA_TYPE_UEYE_USB3_XC:
            type = PyBytes_FromString("USB 3 uEye XC");
            break;
        case IS_CAMERA_TYPE_UEYE_ETH_SE:
            type = PyBytes_FromString("GigE uEye SE");
            break;
        case IS_CAMERA_TYPE_UEYE_ETH_REP:    
            type = PyBytes_FromString("GigE uEye PoE");
            break;
        case IS_CAMERA_TYPE_UEYE_ETH_CP:
            type = PyBytes_FromString("GigE uEye CP");
            break;
        case IS_CAMERA_TYPE_UEYE_ETH_LE:
            type = PyBytes_FromString("GigE uEye LE");
            break;
        case IS_CAMERA_TYPE_UEYE_PMC:
            type = PyBytes_FromString("Virtual multicast camera");
            break;
        default:
            type = PyBytes_FromString("Unknown Camera type");
    }

    PyDict_SetItemString(dict, "serial_num", serial_num);
    PyDict_SetItemString(dict, "manufacturer", manufacturer);
    PyDict_SetItemString(dict, "hw_version", hw_version);
    PyDict_SetItemString(dict, "date", manufacture_date);
    PyDict_SetItemString(dict, "select", select);
    PyDict_SetItemString(dict, "type", type);

    Py_DECREF(serial_num);
    Py_DECREF(manufacturer);
    Py_DECREF(hw_version);
    Py_DECREF(manufacture_date);
    Py_DECREF(select);
    Py_DECREF(type);

    return dict;
}

/*
 * Initialize the newly created object with a camera
 * This means the definition of the camera object is:
 *      def __init__ (self, handle=0)
 */
int camera_init(Camera * self, PyObject * args, PyObject * kwds)
{
    static char *kwlist[] = {"handle", NULL};
    self->handle = 0;

    if (!PyArg_ParseTupleAndKeywords(args,kwds, "|i", kwlist, &self->handle))
    {
        return -1;
    }

    int returnCode = is_InitCamera(&self->handle, NULL);
    switch(returnCode)
    {
        case IS_SUCCESS: 
            break;
        case IS_CANT_OPEN_DEVICE:
            PyErr_SetString(PyExc_IOError, "Camera not connected.");
            return -1;
        default:
            PyErr_Format(PyExc_IOError, "Unable to open camera (Error %d)", returnCode);
            return -1;
    }

    self->status = (int)CONNECTED;

    // Initialize other fields based on the camera info.
    PyObject * camera_info = camera_camera_info(self);
    if (!camera_info)
    {
        return -1;
    }

    PyObject * manufacturer = PyDict_GetItemString(camera_info, "manufacturer");
    if(!manufacturer)
    {
        PyErr_SetString(PyExc_KeyError, "'manufacturer'");
        return -1;
    }

    Py_DECREF(camera_info);

    self->status = (int)READY;

    return 0;
}

/*
 * Returns the current status of the camera in string format
 */
PyObject * camera_status(Camera * self)
{
    PyObject * result = NULL;
    if (self->status == (int)NOT_READY)
    {
        result = Py_BuildValue("s","Not Ready");
    }
    else if(self->status == (int)READY)
    {
        result = Py_BuildValue("s", "Ready");
    }
    else
    {
        result = Py_BuildValue("s", "Connected");
    }
    return result;
} 

/*
 * Declaration of all the publicly accessible attributes of the Camera object
 */
PyMemberDef camera_members[] = {
    {NULL} /* Sentinel */
};

/*
 * Declaration of all the publicly accessible functions of the Camera object
 */
PyMethodDef camera_methods[] = {
    {"status", (PyCFunction)camera_status, METH_NOARGS,
     "Return the current status of the Camera"
    },
    {"camera_info", (PyCFunction)camera_camera_info, METH_NOARGS,
     "Returns a dictionary containing the information about the camera"
    },
    {NULL} /* Sentinel */
};

PyTypeObject ids_CameraType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "ids.Camera",               /* tp_name */
    sizeof(Camera),             /* tp_basicsize */
    0,                          /* tp_itemsize */
    (destructor)camera_dealloc, /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_compare */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "Wrapper object around IDS Camera", /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    camera_methods,            /* tp_methods */
    camera_members,            /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)camera_init,     /* tp_init */
    0,                         /* tp_alloc */
    camera_new,                /* tp_new */
};
