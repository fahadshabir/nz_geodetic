#include <Python.h>
#include "nztm.h"

//static double rad2deg = 180/3.1415926535898; // defining here to avoid touching original source

static char module_docstring[] =
  "A module for converting between New Zealand Transverse Mercator 2000 \
  (NZTM2000) and NZGD2000.\n\n\
  This module is based on source code provided by Land Information New Zealand.";

static PyObject *linz_nztm_geod(PyObject *self, PyObject *args);
static char linz_nztm_to_geod_docstring[] =
  "Convert a point from NZTM2000 to NZGD2000 radians.\n\n\
  nnzmg_to_nztm(easting: float, northing: float) -> (latitude: float, longitude: float)";

static PyObject *linz_geod_nztm(PyObject *self, PyObject *args);
static char linz_geod_to_nztm_docstring[] =
  "Convert a grid reference in NZGD1949 radians to New Zealand Map Grid.\n\n\
  nztm_to_nzmg(latitude: float, longitude: float) -> (easting: float, northing: float)";

static PyMethodDef module_methods[] = {
    {"nztm2000_to_nzgd2000", linz_nztm_geod, METH_VARARGS, linz_nztm_to_geod_docstring},
    {"nzgd2000_to_nztm2000", linz_geod_nztm, METH_VARARGS, linz_geod_to_nztm_docstring},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC init_nztm(void)
{
    PyObject *m = Py_InitModule3("_nztm", module_methods, module_docstring);
    if (m == NULL)
        return;
}

static PyObject *linz_nztm_geod(PyObject *self, PyObject *args)
{
    double n, e, lt, ln; /* northing (m), easting (m), latitude (rad), longitude (rad) */
    PyObject ret;

    if (!PyArg_ParseTuple(args, "dd", &e, &n))
        return NULL;

    nztm_geod(n, e, &lt, &ln);

    {
        PyObject *ret = Py_BuildValue("dd", lt, ln);
        return ret;
    }
}

static PyObject *linz_geod_nztm(PyObject *self, PyObject *args)
{
    double n, e, lt, ln; /* northing (m), easting (m), latitude (rad), longitude (rad) */
    PyObject ret;

    if (!PyArg_ParseTuple(args, "dd", &lt, &ln))
        return NULL;

    geod_nztm(lt, ln, &n, &e);

    {
        PyObject *ret = Py_BuildValue("dd", e, n);
        return ret;
    }
}
