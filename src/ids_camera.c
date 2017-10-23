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
 * Initialize the newly created object with a camera
 */
int camera_init(Camera * self, PyObject * args, PyObject * kwds)
{
    return 0;
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
