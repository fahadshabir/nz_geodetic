#include <Python.h>
#include "nzmg.h"

//static double rad2deg = 180/3.1415926535898; // defining here to avoid touching original source

static char module_docstring[] =
  "A module for converting between New Zealand Map Grid and NZGD1949,\n\
  e.g. latitude/longitude pairs.\n\n\
  This module is based on source code provided by Land Information New Zealand.";

static PyObject *linz_nzmg_geod(PyObject *self, PyObject *args);
static char linz_nzmg_to_geod_docstring[] =
  "Convert a point from New Zealand Map Grid to NZGD1949 radians.\n\n\
  nzmg_to_nzgd1949(easting: float, northing: float) -> (latitude: float, longitude: float)";

static PyObject *linz_geod_nzmg(PyObject *self, PyObject *args);
static char linz_geod_to_nzmg_docstring[] =
  "Convert a grid reference in NZGD1949 radians to New Zealand Map Grid.\n\n\
  nzgd1949_to_nzmg(latitude: float, longitude: float) -> (easting: float, northing: float)";

static PyMethodDef module_methods[] = {
    {"nzmg_to_nzgd1949", linz_nzmg_geod, METH_VARARGS, linz_nzmg_to_geod_docstring},
    {"nzgd1949_to_nzmg", linz_geod_nzmg, METH_VARARGS, linz_geod_to_nzmg_docstring},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC init_nzmg(void)
{
    PyObject *m = Py_InitModule3("_nzmg", module_methods, module_docstring);
    if (m == NULL)
        return;
}

static PyObject *linz_nzmg_geod(PyObject *self, PyObject *args)
{
    double n, e, lt, ln; /* northing (m), easting (m), latitude (rad), longitude (rad) */
    PyObject ret;

    if (!PyArg_ParseTuple(args, "dd", &e, &n))
        return NULL;

    nzmg_geod(n, e, &lt, &ln);

    {
        PyObject *ret = Py_BuildValue("dd", lt, ln);
        return ret;
    }
}

static PyObject *linz_geod_nzmg(PyObject *self, PyObject *args)
{
    double n, e, lt, ln; /* northing (m), easting (m), latitude (rad), longitude (rad) */
    PyObject ret;

    if (!PyArg_ParseTuple(args, "dd", &lt, &ln))
        return NULL;

    geod_nzmg(lt, ln, &n, &e);

    {
        PyObject *ret = Py_BuildValue("dd", e, n);
        return ret;
    }
}
