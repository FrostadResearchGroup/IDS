#pragma once

#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

#include <Python.h>

#if PY_MAJOR_VERSION >= 3
#include <stdint.h>
#else
#include "stdint.h"
#endif

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

enum Status
{
    NOT_READY,
    CONNECTED,
    READY,
};

PyMethodDef idsMethods[] =
{
    {NULL, NULL, 0, NULL} /* sentinel */
};

#endif
