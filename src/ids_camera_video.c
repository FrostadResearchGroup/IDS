#include <uEye.h>
#include <uEye_tools.h>
#include "ids.h"

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
    argList = Py_BuildValue("i", videoID);
    result = PyObject_CallObject((PyObject *) &ids_VideoType, argList);
    Py_DECREF(argList);
    return result;
}

int video_init(Video * self, PyObject * args, PyObject * kwds)
{
    static char * kwlist[] = {"id", NULL};
    if (!PyArg_ParseTupleAndKeywords(args,kwds, "i", kwlist, &self->videoID))
    {
        return -1;
    }

    return 0;
}

/**
  * Initializes the attributes of the newly created Video object to 0
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

/**
  * Declaration of all the pubclicly accessible functions of the Video Object
  */  
PyMethodDef video_methods[] = {
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
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)video_init,      /* tp_init */
    0,                         /* tp_alloc */
    video_new,                 /* tp_new */
};

