#pragma once

#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

#include <Python.h>

#if PY_MAJOR_VERSION >= 3
#include <stdint.h>
#else
#include "stdint.h"
#endif

/*
 * Struct that defines the underlying Camera class
 */
typedef struct
{
    PyObject_HEAD
    HIDS        handle;
    uint32_t    width;
    uint32_t    height;
    int         bitdepth;
    int         color;
    int         autofeatures;
    int         status;

} Camera;

/*
 * Enum defining the current status of the camera
 */
enum Status
{
    NOT_READY,
    CONNECTED,
    READY,
};

/*
 * Methods for the ids Module
 */
extern PyMethodDef idsMethods[];

extern PyTypeObject ids_CameraType;
extern PyMethodDef camera_methods[];

#endif
