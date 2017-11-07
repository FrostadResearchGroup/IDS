#include <uEye.h>
#include "ids.h"
#include "structmember.h"
#include <string.h>
#include <wchar.h>

extern PyObject * get_gain(Camera * self, int command);
extern PyObject * camera_get_image(Camera * self);
extern PyObject * camera_video(Camera * self);

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
    if (self->status == READY)
    {
        is_ExitImageQueue(self->handle);
    }
    is_ExitCamera(self->handle);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

/*
 * Obtain the hard coded data in the non-volatile camera memory
 * @return A Python Dictionary that contains key-value pairs of this data
 * @note The keys of the dictionary are as follow:
 *      serial_num   : The serial number of the camera
 *      manufacturer : The manufacturer of the camera
 *      date         : The manufacturing date of the camera
 *      id           : Customizable id of the camera
 *      type         : Type of the camera
 */
PyObject * camera_camera_info(Camera * self)
{
    CAMINFO cam_info;
    PyObject * serial_num;
    PyObject * manufacturer;
    PyObject * hw_version;
    PyObject * manufacture_date;
    PyObject * select;
    PyObject * type;
    PyObject * dict = PyDict_New();

    int returnCode = is_GetCameraInfo(self->handle, &cam_info);
    if (returnCode != IS_SUCCESS)
    {
        print_error(self);
        return NULL;
    }

    serial_num = PyBytes_FromString(cam_info.SerNo);
    manufacturer = PyBytes_FromString(cam_info.ID);
    hw_version = PyBytes_FromString(cam_info.Version);
    manufacture_date = PyBytes_FromString(cam_info.Date);
    select = Py_BuildValue("b", cam_info.Select);

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
    PyDict_SetItemString(dict, "id", select);
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
 * Returns the info about the sensor used in the camera
 * @return Python Dictionary for the sensor info
 */
PyObject * camera_sensor_info(Camera * self)
{
    SENSORINFO sensor_info;
   
    PyObject * sensor_id;
    PyObject * sensor_name;
    PyObject * max_width;
    PyObject * max_height;
    PyObject * master_gain;
    PyObject * red_gain;
    PyObject * green_gain;
    PyObject * blue_gain;
    PyObject * global_shutter;
    PyObject * pixel_size; 
    PyObject * first_pixel_color;
    PyObject * color_mode;
    PyObject * dict = PyDict_New();
    
    int returnCode = is_GetSensorInfo(self->handle, &sensor_info);
    if (returnCode!= IS_SUCCESS)
    {
        print_error(self);
        return NULL;
    }

    sensor_id = Py_BuildValue("h", sensor_info.SensorID);
    sensor_name = PyBytes_FromString(sensor_info.strSensorName);
    max_width = Py_BuildValue("i", sensor_info.nMaxWidth);
    max_height = Py_BuildValue("i", sensor_info.nMaxHeight);
    master_gain = Py_BuildValue("O", sensor_info.bMasterGain ? Py_True : Py_False);
    red_gain = Py_BuildValue("O", sensor_info.bRGain ? Py_True : Py_False);
    green_gain = Py_BuildValue("O", sensor_info.bGGain ? Py_True : Py_False);
    blue_gain = Py_BuildValue("O", sensor_info.bBGain ? Py_True : Py_False);
    global_shutter = Py_BuildValue("O", sensor_info.bGlobShutter ? Py_True : Py_False); 
    pixel_size = Py_BuildValue("d", sensor_info.wPixelSize / 100.0);

    switch(sensor_info.nColorMode)
    {
        case IS_COLORMODE_MONOCHROME:
            color_mode = PyBytes_FromString("Monochrome");
            break;
        case IS_COLORMODE_BAYER:
            color_mode = PyBytes_FromString("Bayer");
            break;
        case IS_COLORMODE_CBYCRY:
            color_mode = PyBytes_FromString("CBYCRY");
            break;
        case IS_COLORMODE_JPEG:
            color_mode = PyBytes_FromString("JPEG");
            break;
        default:
            color_mode = PyBytes_FromString("Invalid");
    }

    switch(sensor_info.nUpperLeftBayerPixel)
    {
        case BAYER_PIXEL_RED:
            first_pixel_color = PyBytes_FromString("Red");
            break;
        case BAYER_PIXEL_GREEN:
            first_pixel_color = PyBytes_FromString("Green");
            break;
        case BAYER_PIXEL_BLUE:
            first_pixel_color = PyBytes_FromString("Blue");
            break;
    }

    PyDict_SetItemString(dict, "sensor_id", sensor_id);
    PyDict_SetItemString(dict, "sensor_name", sensor_name);
    PyDict_SetItemString(dict, "max_width", max_width);
    PyDict_SetItemString(dict, "max_height", max_height);
    PyDict_SetItemString(dict, "master_gain", master_gain);
    PyDict_SetItemString(dict, "red_gain", red_gain);
    PyDict_SetItemString(dict, "green_gain", green_gain);
    PyDict_SetItemString(dict, "blue_gain", blue_gain);
    PyDict_SetItemString(dict, "global_shutter", global_shutter);
    PyDict_SetItemString(dict, "pixel_size", pixel_size);
    PyDict_SetItemString(dict, "first_pixel_color", first_pixel_color);
    PyDict_SetItemString(dict, "color_mode", color_mode);

    Py_DECREF(sensor_id);
    Py_DECREF(sensor_name);
    Py_DECREF(max_width);
    Py_DECREF(max_height);
    Py_DECREF(master_gain);
    Py_DECREF(red_gain);
    Py_DECREF(green_gain);
    Py_DECREF(blue_gain);
    Py_DECREF(global_shutter);
    Py_DECREF(pixel_size);
    Py_DECREF(first_pixel_color);
    Py_DECREF(color_mode);

    return dict;
}

PyObject * camera_save_settings(Camera * self, PyObject * args)
{
    int returnCode;
    wchar_t * filename;
    int len;

    if (!PyArg_ParseTuple(args,"|s", &filename))
    {
        return NULL;
    }

    len = strlen(filename) + 1;

    returnCode = is_ParameterSet(self->handle, IS_PARAMETERSET_CMD_SAVE_FILE, (void *)filename, NULL);
    if (returnCode != IS_SUCCESS)
    {
        print_error(self);
        return NULL;
    }
    Py_RETURN_NONE;
}

PyObject * camera_load_settings(Camera * self, PyObject * args)
{
    int returnCode;
    wchar_t * filename;
    int len;
    if (!PyArg_ParseTuple(args, "|s", &filename))
    {
        return NULL;
    }

    if (filename == NULL)
    {
        return NULL;
    }

    len = strlen(filename) + 1;

    returnCode = is_ParameterSet(self->handle, IS_PARAMETERSET_CMD_LOAD_FILE, (void *)filename, NULL);
    if (returnCode != IS_SUCCESS)
    {
        print_error(self);
        return NULL;
    }
    Py_RETURN_NONE;
}

PyObject * camera_get_aoi(Camera * self)
{
    int returnCode;
    PyObject * x;
    PyObject * y;
    PyObject * height;
    PyObject * width;
    PyObject * dict = PyDict_New();

    IS_RECT rectAOI;
    returnCode = is_AOI(self->handle, IS_AOI_IMAGE_GET_AOI, (void *)&rectAOI, sizeof(rectAOI));
    if (returnCode != IS_SUCCESS)
    {
        print_error(self);
        return NULL;
    }

    x = Py_BuildValue("i", rectAOI.s32X);
    y = Py_BuildValue("i", rectAOI.s32Y);
    width = Py_BuildValue("i", rectAOI.s32Width);
    height = Py_BuildValue("i", rectAOI.s32Height);

    PyDict_SetItemString(dict, "x", x);
    PyDict_SetItemString(dict, "y", y);
    PyDict_SetItemString(dict, "height", height);
    PyDict_SetItemString(dict, "width", width);

    return dict;
}

PyObject * camera_set_aoi(Camera * self, PyObject * args, PyObject * kwds)
{
    static char *kwlist[] = {"x", "y", "width", "height", NULL};
    int returnCode;
    int x, y, width, height;
    IS_RECT rectAOI;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "iiii", kwlist, &x, &y, &width, &height))
    {
		PyErr_SetString(PyExc_TypeError, "Parsing didn't work");
        return NULL;
    }

    rectAOI.s32X = x;
    rectAOI.s32Y = y;
    rectAOI.s32Width = width;
    rectAOI.s32Height = height;

    returnCode = is_AOI(self->handle, IS_AOI_IMAGE_SET_AOI, (void * )&rectAOI, sizeof(rectAOI));
    if (returnCode != IS_SUCCESS)
    {
        print_error(self);
        return NULL;
    }

    Py_RETURN_NONE;
}

/*
 * Initialize the newly created object with a camera
 * This means the definition of the camera object is:
 *      def __init__ (self, handle=0)
 */
int camera_init(Camera * self, PyObject * args, PyObject * kwds)
{
    static char *kwlist[] = {"handle", NULL};
    int returnCode;
    PyObject * camera_info;
    PyObject * manufacturer;
    PyObject * width;
    PyObject * height;
    PyObject * sensor_info;

    self->handle = 0;
    
    if (!PyArg_ParseTupleAndKeywords(args,kwds, "|i", kwlist, &self->handle))
    {
        return -1;
    }

    returnCode = is_InitCamera(&self->handle, NULL);
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
    camera_info = camera_camera_info(self);
    if (!camera_info)
    {
        return -1;
    }

    manufacturer = PyDict_GetItemString(camera_info, "manufacturer");
    if(!manufacturer)
    {
        PyErr_SetString(PyExc_KeyError, "'manufacturer'");
        return -1;
    }

    sensor_info = camera_sensor_info(self);
    if (!sensor_info)
    {
        return -1;
    }

    width = PyDict_GetItemString(sensor_info, "max_width");
    if (!width)
    {
        PyErr_SetString(PyExc_KeyError, "'max_width'");
        return -1;
    }

    height = PyDict_GetItemString(sensor_info, "max_height");
    if (!height)
    {
        PyErr_SetString(PyExc_KeyError, "'max_height'");
        return -1;
    }
    self->width = PyLong_AsLong(width);
    self->height = PyLong_AsLong(height);

    returnCode = is_InitImageQueue(self->handle, 0);
    if (returnCode != IS_SUCCESS)
    {
        print_error(self);
        return -1;
    }

    Py_DECREF(camera_info);

    self->status = (int)READY;

    return 0;
}

PyObject * camera_default_gain(Camera * self)
{
    PyObject * default_master_gain;
    PyObject * default_red_gain;
    PyObject * default_green_gain;
    PyObject * default_blue_gain;
    PyObject * dict = PyDict_New();

    default_master_gain = get_gain(self, IS_GET_DEFAULT_MASTER);
    default_red_gain = get_gain(self, IS_GET_DEFAULT_RED);
    default_green_gain = get_gain(self, IS_GET_DEFAULT_GREEN);
    default_blue_gain = get_gain(self, IS_GET_DEFAULT_BLUE);

    PyDict_SetItemString(dict, "master", default_master_gain);
    PyDict_SetItemString(dict, "red", default_red_gain);
    PyDict_SetItemString(dict, "blue", default_blue_gain);
    PyDict_SetItemString(dict, "green", default_green_gain);
    return dict;
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
    {"sensor_info", (PyCFunction)camera_sensor_info, METH_NOARGS,
     "Returns a dictionary containing the information about the sensor"
    },
    {"default_gain", (PyCFunction) camera_default_gain, METH_NOARGS,
     "Returns a dictionary containing the default gain values"
    },
    {"save_settings", (PyCFunction) camera_save_settings, METH_VARARGS,
     "Save camera settings to a file"
    },
    {"load_settings", (PyCFunction) camera_load_settings, METH_VARARGS,
     "Load camera settings from a file"
    },
    {"set_aoi", (PyCFunction) camera_set_aoi, METH_VARARGS | METH_KEYWORDS,
     "Set Area of Interest"
    },
    {"get_aoi", (PyCFunction) camera_get_aoi, METH_VARARGS,
     "Get Area of Interest"
    },
    {"get_image", (PyCFunction) camera_get_image, METH_NOARGS,
     "Get the next image waiting in queue"
    },
    {"video", (PyCFunction) camera_video, METH_NOARGS,
     "Get the video object"
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
    camera_properties,         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)camera_init,     /* tp_init */
    0,                         /* tp_alloc */
    camera_new,                /* tp_new */
};
