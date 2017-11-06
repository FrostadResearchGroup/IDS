#include <uEye.h>
#include "ids.h"

#define IMAGE_TIMEOUT 1000

int camera_wait_for_image(Camera * self, char ** ppBuffer, INT * pImgID, int * pRetVal)
{
    int retCode;

    retCode = is_WaitForNextImage(self->handle, IMAGE_TIMEOUT, ppBuffer, pImgID);
    if (retCode != IS_SUCCESS)
    {
        print_error(self);
        return -1;
    }
    return 0;
}

int camera_unlock_image_queue_buffer(Camera * self, INT imgID, char * pBuffer)
{
    int retCode;
    retCode = is_UnlockSeqBuf(self->handle, nMemID, mem); 
    if (retCode != IS_SUCCESS)
    {
        print_error(self);
    }
    return retCode;
}

PyObject * camera_get_image_info(Camera * self, INT imgID, int * pRetVal)
{
    return NULL;
}

PyObject * camera_get_image_as_ndarray(Camera * self, char * pBuffer, int * pRetVal)
{
    return NULL;
}

PyObject * camera_get_image(Camera * self)
{
    int retCode;
    INT nMemID = 0;
    char * pBuffer = NULL;
    PyObject * img;
    PyObject * image_info;
    PyObject * returnObj;

    retCode = camera_wait_for_image(self, &pBuffer, &nMemID);
    if (retCode != 0)
    {
        print_error(self);
        return NULL;
    }

    img = camera_get_image_as_ndarray(self, pBuffer, &retCode);
    if (retCode != 0)
    {
        return NULL;
    }

    image_info = camera_get_image_info(self, nMemID, &retCode);
    if (retCode != 0)
    {
        Py_DECREF(img);
        return NULL;
    }

    retCode = camera_unlock_image_queue_buffer(self, nMemID, pBuffer);
    if (retCode != IS_SUCCESS)
    {
        Py_DECREF(img);
        Py_DECREF(image_info);
        return NULL;
    }

    returnObject = Py_BuildValue("(OO)", img, image_info);

    Py_DECREF(img);
    Py_DECREF(image_info);
   
    return returnObject;
}
