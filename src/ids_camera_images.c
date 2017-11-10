#include <uEye.h>
#include "ids.h"
#include <datetime.h>

#define IMAGE_TIMEOUT 1000

int camera_wait_for_image(Camera * self, char ** ppBuffer, INT * pImgID)
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
    retCode = is_UnlockSeqBuf(self->handle, imgID, pBuffer); 
    if (retCode != IS_SUCCESS)
    {
        print_error(self);
    }
    return retCode;
}

PyObject * camera_get_image_info(Camera * self, INT imgID, int * pRetVal)
{
    UEYEIMAGEINFO imageInfo;
    int returnCode;
    PyObject * info = PyDict_New();
    PyObject * timestamp;
    PyObject * digital_input;
    PyObject * gpio1;
    PyObject * gpio2;
    PyObject * frame_number;
    PyObject * camera_buffers;
    PyObject * used_camera_buffers;
    PyObject * height;
    PyObject * width;

    returnCode = is_GetImageInfo(self->handle, imgID, &imageInfo, sizeof(imageInfo));
    if (returnCode != IS_SUCCESS)
    {
        print_error(self);
        return NULL;
    }

    timestamp = PyDateTime_FromDateAndTime(imageInfo.TimestampSystem.wYear, imageInfo.TimestampSystem.wMonth, imageInfo.TimestampSystem.wDay, imageInfo.TimestampSystem.wHour, imageInfo.TimestampSystem.wMinute, imageInfo.TimestampSystem.wSecond, imageInfo.TimestampSystem.wMilliseconds);
    digital_input = Py_BuildValue("I", imageInfo.dwIoStatus&4);
    gpio1 = Py_BuildValue("I", imageInfo.dwIoStatus&2);
    gpio2 = Py_BuildValue("I", imageInfo.dwIoStatus&1);
    frame_number = Py_BuildValue("K", imageInfo.u64FrameNumber);
    camera_buffers = Py_BuildValue("I", imageInfo.dwImageBuffers);
    used_camera_buffers = Py_BuildValue("I", imageInfo.dwImageBuffersInUse);
    height = Py_BuildValue("I", imageInfo.dwImageHeight);
    width = Py_BuildValue("I", imageInfo.dwImageWidth);

    PyDict_SetItemString(info, "timestamp", timestamp);
    PyDict_SetItemString(info, "digital_input", digital_input);
    PyDict_SetItemString(info, "gpio1", gpio1);
    PyDict_SetItemString(info, "gpio2", gpio2);
    PyDict_SetItemString(info, "frame_number", frame_number);
    PyDict_SetItemString(info, "camera_buffers", camera_buffers);
    PyDict_SetItemString(info, "used_camera_buffers", used_camera_buffers);
    PyDict_SetItemString(info, "height", height);
    PyDict_SetItemString(info, "width", width);

    Py_DECREF(timestamp);
    Py_DECREF(digital_input);
    Py_DECREF(gpio1);
    Py_DECREF(gpio2);
    Py_DECREF(frame_number);
    Py_DECREF(camera_buffers);
    Py_DECREF(used_camera_buffers);
    Py_DECREF(height);
    Py_DECREF(width);

    return info;
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

    returnObj = Py_BuildValue("(OO)", img, image_info);

    Py_DECREF(img);
    Py_DECREF(image_info);
   
    return returnObj;
}
