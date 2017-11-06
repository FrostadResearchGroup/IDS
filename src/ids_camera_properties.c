#include <uEye.h>
#include "ids.h"
#include "structmember.h"
#include <string.h>

/**
  * Common wrapper around is_SetHardwareGain used to set master, red, green and blue gain
  * @arg self Pointer to the Camera object
  * @arg master_gain The new value of the master gain
  * @arg red_gain The new value of red gain
  * @arg green_gain The new value of green gain
  * @arg blue_gain The new value of blue gain
  *
  * @return The returnCode of the function call is_SetHardwareGain produced by IDS library
  */ 
int set_gain(Camera * self, int master_gain, int red_gain, int green_gain, int blue_gain)
{
    return is_SetHardwareGain(self->handle, master_gain, red_gain, green_gain, blue_gain);
}

/**
  * Commone wrapper around is_SetHardwareGain used to get master/red/green/blue gain
  * @arg self Pointer to the camera object
  * @arg command The command specifying which gain to get
  *
  * @note For more information please look at https://www.ids-imaging.us/manuals/uEye_SDK/EN/uEye_Manual_4.90/index.html?is_initcamera.html
  *
  * @return The gain value as a Python Object
  */ 
PyObject * get_gain(Camera * self, int command)
{
    int val = is_SetHardwareGain(self->handle, command, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);

    return Py_BuildValue("i", val);
}

int camera_set_master_gain(Camera * self, PyObject * value, void * closure)
{
    int master_gain;
    char * converted_val;
    int red_gain = IS_IGNORE_PARAMETER;
    int blue_gain = IS_IGNORE_PARAMETER;
    int green_gain = IS_IGNORE_PARAMETER;
    int returnCode;

    if (value == NULL)
    {
        PyErr_SetString(PyExc_IOError, "Master gain can't be null");
        return -1;
    }

    // Check if the argument passed in was "auto"
    if (check_is_string(value))
    {
        converted_val = get_as_string(value);
        if( strcmp(converted_val, "auto") != 0 )
        {
            PyErr_SetString(PyExc_IOError, "Invalid argument");
            return -1;
        }
        master_gain = IS_SET_ENABLE_AUTO_GAIN;
    }
    master_gain = (int)PyLong_AsLong(value);
    if (master_gain == -1 || master_gain < 0 || master_gain > 100)
    {
        PyErr_SetString(PyExc_IOError, "Invalid argument");
        return -1;
    }

    returnCode = set_gain(self, master_gain, red_gain, green_gain, blue_gain);
    if (returnCode != IS_SUCCESS)
    {
        print_error(self);
        return -1;
    }

    return 0;
}

PyObject * camera_get_master_gain(Camera * self, void * closure)
{
    return get_gain(self, IS_GET_MASTER_GAIN);
}

int camera_set_red_gain(Camera * self, PyObject * value, void * closure)
{
    int master_gain = IS_IGNORE_PARAMETER;
    int red_gain = IS_IGNORE_PARAMETER;
    int blue_gain = IS_IGNORE_PARAMETER;
    int green_gain = IS_IGNORE_PARAMETER;
    int returnCode;

    if (value == NULL)
    {
        PyErr_SetString(PyExc_IOError, "Blue gain can't be null");
        return -1;
    }

    red_gain = (int)PyLong_AsLong(value);
    if (red_gain == -1 || red_gain < 0 || red_gain > 100)
    {
        PyErr_SetString(PyExc_IOError, "Invalid argument");
        return -1;
    }

    returnCode = set_gain(self, master_gain, red_gain, green_gain, blue_gain);
    if (returnCode != IS_SUCCESS)
    {
        print_error(self);
        return -1;
    }

    return 0;
}

PyObject * camera_get_red_gain(Camera * self, void * closure)
{
    return get_gain(self, IS_GET_RED_GAIN);
}

int camera_set_green_gain(Camera * self, PyObject * value, void * closure)
{
    int master_gain = IS_IGNORE_PARAMETER;
    int red_gain = IS_IGNORE_PARAMETER;
    int blue_gain = IS_IGNORE_PARAMETER;
    int green_gain = IS_IGNORE_PARAMETER;
    int returnCode;

    if (value == NULL)
    {
        PyErr_SetString(PyExc_IOError, "Green gain can't be null");
        return -1;
    }

    green_gain = (int)PyLong_AsLong(value);
    if (green_gain == -1 || green_gain < 0 || green_gain > 100)
    {
         PyErr_SetString(PyExc_IOError, "Invalid argument");
         return -1;
    }

    returnCode = set_gain(self, master_gain, red_gain, green_gain, blue_gain);
    if (returnCode != IS_SUCCESS)
    {
        print_error(self);
        return -1;
    }

    return 0;
}

PyObject * camera_get_green_gain(Camera * self, void * closure)
{
    return get_gain(self, IS_GET_GREEN_GAIN);
}

int camera_set_blue_gain(Camera * self, PyObject * value, void * closure)
{
    int master_gain = IS_IGNORE_PARAMETER;
    int red_gain = IS_IGNORE_PARAMETER;
    int blue_gain = IS_IGNORE_PARAMETER;
    int green_gain = IS_IGNORE_PARAMETER;
    int returnCode;

    if (value == NULL)
    {
        PyErr_SetString(PyExc_IOError, "Blue gain can't be null");
        return -1;
    }

    blue_gain = (int)PyLong_AsLong(value);
    if (blue_gain == -1 || blue_gain < 0 || blue_gain > 100)
    {
        PyErr_SetString(PyExc_IOError, "Invalid argument");
        return -1;
    }

    returnCode = set_gain(self, master_gain, red_gain, green_gain, blue_gain);
    if (returnCode != IS_SUCCESS)
    {
        print_error(self);
        return -1;
    }

    return 0;
}

PyObject * camera_get_blue_gain(Camera * self, void * closure)
{
    return get_gain(self, IS_GET_BLUE_GAIN);
}

int camera_set_frame_rate(Camera * self, PyObject * value, void * closure)
{
    int returnCode;
    double setVal;
    double wantedVal;

    wantedVal = PyFloat_AsDouble(value);
    returnCode = is_SetFrameRate(self->handle, wantedVal, &setVal);
    if (returnCode != IS_SUCCESS)
    {
        print_error(self);
        return -1;
    }
    return 0;
}

PyObject * camera_get_frame_rate(Camera * self, void * closure)
{
    int returnCode;
    double val;
    PyObject * value;

    returnCode = is_SetFrameRate(self->handle, IS_GET_FRAMERATE, &val);
    if (returnCode != IS_SUCCESS)
    {
        print_error(self);
        return NULL;
    }
    
    value = PyFloat_FromDouble(val);
    return value;
}

int camera_set_pixel_clock(Camera * self, PyObject * value, void * closure)
{
    int returnCode;
    UINT nPixelClock;

    if (value == NULL)
    {
        return -1;
    }

    if (PyLong_Check(value))
    {
        nPixelClock = (UINT)PyLong_AsLong(value);
        returnCode = is_PixelClock(self->handle, IS_PIXELCLOCK_CMD_SET,(void*)&nPixelClock, sizeof(nPixelClock));
        if (returnCode != IS_SUCCESS)
        {
            print_error(self);
            return -1;
        }
    }
    else
    {
        return -1;
    }

    return 0;
}

PyObject * camera_get_pixel_clock(Camera * self, void * closure)
{
    int returnCode;
    UINT nPixelClock;

    returnCode = is_PixelClock(self->handle, IS_PIXELCLOCK_CMD_GET, (void*)&nPixelClock, sizeof(nPixelClock));
    if (returnCode != IS_SUCCESS)
    {
        print_error(self);
        return NULL;
    }
    
    return Py_BuildValue("i", nPixelClock);
}

int camera_set_exposure(Camera * self, PyObject * value, void * clousre)
{
    int returnCode;
    double exposure_time;

    if (value == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "Exposure can not be set to NULL");
        return -1;
    }

    if (PyFloat_Check(value))
    {
        exposure_time = PyFloat_AsDouble(value);
        returnCode = is_Exposure(self->handle, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&exposure_time, sizeof(exposure_time));
        if (returnCode != IS_SUCCESS)
        {
            print_error(self);
            return -1;
        }
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "Exposure must be an integer or a double");
        return -1;
    }

    return 0;
}

PyObject * camera_get_exposure(Camera * self, void * closure)
{
    int returnCode;
    double exposure_time;
    returnCode = is_Exposure(self->handle, IS_EXPOSURE_CMD_GET_EXPOSURE, (void*)&exposure_time, sizeof(exposure_time));
    if (returnCode != IS_SUCCESS)
    {
        print_error(self);
        return NULL;
    }
    return Py_BuildValue("d", exposure_time);
}

int camera_set_white_balance(Camera * self, PyObject * value, void * closure)
{
    int returnCode;
    UINT nType;

    if (value == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "White balance can not be set to NULL");
        return -1;
    }

    if (PyLong_Check(value))
    {
        nType = (UINT)(PyLong_AsLong(value));
        returnCode = is_AutoParameter(self->handle,IS_AWB_CMD_SET_TYPE, (void*)&nType, sizeof(nType));
        if (returnCode != IS_SUCCESS)
        {
            print_error(self);
            return -1;
        }
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "White balance must be an integer");
        return -1;
    }

    return 0;
}

PyObject * camera_get_white_balance(Camera * self, void * closure)
{
    int returnCode;
    UINT nType = 0;

    returnCode = is_AutoParameter(self->handle, IS_AWB_CMD_GET_TYPE, (void*)nType, sizeof(nType));
    if (returnCode != IS_SUCCESS)
    {
        print_error(self);
        return NULL;
    }
    return Py_BuildValue("i", nType);
}

PyGetSetDef camera_properties[] = {
    {"master_gain", (getter)camera_get_master_gain, (setter)camera_set_master_gain, "Master Gain", NULL},
    {"red_gain", (getter)camera_get_red_gain, (setter)camera_set_red_gain, "Red Gain", NULL},
    {"blue_gain", (getter)camera_get_blue_gain, (setter)camera_set_blue_gain, "Blue Gain", NULL},
    {"green_gain", (getter)camera_get_green_gain, (setter)camera_set_green_gain, "Green Gain", NULL},
    {"frame_rate", (getter)camera_get_frame_rate, (setter)camera_set_frame_rate, "Frame Rate", NULL},
    {"pixel_clock", (getter)camera_get_pixel_clock, (setter)camera_set_pixel_clock, "Pixel Clock", NULL},
    {"exposure", (getter)camera_get_exposure, (setter)camera_set_exposure, "Exposure Time", NULL},
    {"white_balance", (getter)camera_get_white_balance, (setter)camera_set_white_balance, "Auto White Balance", NULL},
    {NULL} /* sentinel */
};
