#pragma once

#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

#include <Python.h>
#include <stdint.h>

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
