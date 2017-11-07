#pragma once

#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

#include <Python.h>

#if PY_MAJOR_VERSION >= 3
#define IS_PY3
#endif

#ifdef IS_PY3
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

/**
  * Struct that defines the Video class
  */
typedef struct
{
    PyObject_HEAD
    int         videoID;
} Video;

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

/*
 * Data Structures for the Camera Object
 */
extern PyTypeObject ids_CameraType;
extern PyMethodDef camera_methods[];
extern PyGetSetDef camera_properties[];

/**
  * Data Structures for the Video Object
  */ 
extern PyTypeObject ids_VideoType;
extern PyMethodDef video_methods[];

void print_error(Camera * self);

/* IDS Exception Objects */
extern PyObject * IDSError;

/* Wrapper functions for converting Python Objects to string and supporting both Python 2 and Python 3 */
extern int check_is_string(PyObject * value);
extern char * get_as_string(PyObject * value);

#endif
