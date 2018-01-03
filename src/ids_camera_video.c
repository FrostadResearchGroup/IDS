#include <uEye.h>
#include <uEye_tools.h>
#include "ids.h"
#include "structmember.h"
#include <string.h>

#define DEFAULT_FRAME_RATE 25.0

extern int display_mode_command(HIDS handle, int command);

void start_video_capture(Video * self)
{
    int returnCode;
    int error = 0;

    while(self->is_capture)
    {
        // TODO: Call AddFrame function
        if (returnCode != IS_AVI_NO_ERR)
        {
            error = 1;
            break;
        }
    }

    if (error)
    {
        printf("Error occured whilst capturing video");
    }
}

int set_frame_rate(Video * self)
{
    return isavi_SetFrameRate(self->videoID, self->frame_rate);
}

int check_display_mode(Video * self)
{
    printf("WARNING: Changing display mode to Device Independent Bitmap(DIB)\n");
    return display_mode_command(self->handle, IS_SET_DM_DIB);
}

/**
  * Function to return a Video object
  */ 
PyObject * camera_video(Camera * self)
{
    PyObject * result = NULL;
    PyObject * argList;
    INT videoID;
    int returnCode;

    returnCode = isavi_InitAVI(&videoID, self->handle);
    if (returnCode != IS_AVI_NO_ERR)
    {
        print_error(self);
        return result;
    }
    argList = Py_BuildValue("(ii)", self->handle, videoID);
    result = PyObject_CallObject((PyObject *) &ids_VideoType, argList);
    Py_DECREF(argList);
    return result;
}

PyObject * video_start(Video * self)
{
    int returnCode;
    returnCode = check_display_mode(self);
    if (returnCode != IS_AVI_NO_ERR)
    {
        return -1;
    }
    returnCode = isavi_OpenAVI(self->videoID, &self->filename);
    if (returnCode != IS_AVI_NO_ERR)
    {
        return -1;
    }

    returnCode = set_frame_rate(self);
    if (returnCode != IS_AVI_NO_ERR)
    {
        return -1;
    }

    self->is_capture = 1;

    // TODO: Make this async
    start_video_capture(self);

    Py_RETURN_NONE;
}

PyObject * video_stop(Video * self)
{
    int returnCode;
    self->is_capture = 0;
    returnCode = isavi_StopAVI(self->videoID);
    if (returnCode != IS_AVI_NO_ERR)
    {
        printf("Failed to stop video capture\n");
        return -1;
    }

    returnCode = isavi_CloseAVI(self->videoID);
    if (returnCode != IS_AVI_NO_ERR)
    {
        printf("Failed to close video capture\n");
        return -1;
    }

    Py_RETURN_NONE;
}

int video_init(Video * self, PyObject * args, PyObject * kwds)
{
    static char * kwlist[] = {"handle", "id", NULL};
    if (!PyArg_ParseTupleAndKeywords(args,kwds, "ii", kwlist, &self->handle, &self->videoID))
    {
        return -1;
    }

    return 0;
}

/**
  * Initializes the attributes of the newly created Video object
  */ 
PyObject * video_new
(
    PyTypeObject * type,
    PyObject * args,
    PyObject * kwds
)
{
    Video * self;

    self = (Video * )type->tp_alloc(type, 0);
    if (self != NULL)
    {
        self->videoID = 0;
    }
    self->handle = -1;
    self->frame_rate = DEFAULT_FRAME_RATE;
    self->is_capture = 0;
    return (PyObject *)self;
}

/**
  * Releases the avi file engine
  */ 
void video_dealloc(Video * self)
{
    isavi_ExitAVI(self->videoID);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyObject * video_get_frame_rate(Video * self, void * closure)
{
    return Py_BuildValue("d", self->frame_rate);
}

int video_set_frame_rate(Video * self, PyObject * value, void * closure)
{
    double val;
    if (self->is_capture)
    {
        PyErr_SetString(PyExc_IOError, "Can't set frame rate during capture");
        return -1;
    }

    val = PyFloat_AsDouble(value);
    self->frame_rate = val;

    return 0;
}

PyObject * video_get_filename(Video * self, void * clousre)
{
    return Py_BuildValue("s", self->filename);
}

int video_set_filename(Video * self, PyObject * value, void * closure)
{
    char * filename;
    if (check_is_string(value))
    {
        filename = get_as_string(value);
        strcpy(self->filename, filename);
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "Invalid type for filename");
        return -1;
    }
}

/**
  * Declaration of all the pubclicly accessible functions of the Video Object
  */  
PyMethodDef video_methods[] = {
    {"start",(PyCFunction)video_start, METH_NOARGS,
      "Start the video capture"
    },
    {"stop",(PyCFunction)video_stop, METH_NOARGS,
      "Stop the video capture"
    }, 
    {NULL} /* Sentinel */
};

/**
  * Declaration of all the publicly accessible properties of the Video Object
  */ 
PyGetSetDef video_properties[] = {
    {"frame_rate", (getter)video_get_frame_rate, (setter)video_set_frame_rate, "Frame Rate", NULL},
    {"filename", (getter)video_get_filename, (setter)video_set_filename, "Filename", NULL},
    {NULL} /* Sentinel */
};

PyTypeObject ids_VideoType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "ids.Video",               /* tp_name */
    sizeof(Video),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)video_dealloc, /* tp_dealloc */
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
    "Wrapper object around IDS Camera Video", /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    video_methods,             /* tp_methods */
    0,                         /* tp_members */
    video_properties,          /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)video_init,      /* tp_init */
    0,                         /* tp_alloc */
    video_new,                 /* tp_new */
};
