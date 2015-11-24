#include <Python.h>
#include "linzgrid.h"
#include "linzgridnzgd2k.h"

#define NZGD49_TO_NZGD2000 0
#define NZGD2000_TO_NZGD49 1

static char module_docstring[] =
  "A module for converting between NZGD1949 and NZGD2000.\
  \n\n\
  This module is based on source code provided by Land Information New Zealand.";

static PyObject *linz_nzgd1949_nzgd2000(PyObject *self, PyObject *args);
static char linz_nzgd1949_nzgd2000_docstring[] =
  "Convert a point from NZGD1949 to NZGD2000.\
  \n\n\
  nzgd1949_nzgd2000(latitude: float, longitude: float) -> Tuple[float, float]\
  \n\n\
  where arguments represent (latitude, longitude) in *radians*.";

static PyObject *linz_nzgd2000_nzgd1949(PyObject *self, PyObject *args);
static char linz_linz_nzgd2000_nzgd1949_docstring[] =
  "Convert a point from NZGD2000 to NZGD1949.\
  \n\n\
  nzgd2000_nzgd1949(latitude: float, longitude: float) -> Tuple[float, float]\
  \n\n\
  where arguments represent (latitude, longitude) in *radians*.";

static PyMethodDef module_methods[] = {
    {"nzgd1949_to_nzgd2000", linz_nzgd1949_nzgd2000_docstring, METH_VARARGS, linz_nzgd1949_nzgd2000_docstring},
    {"nzgd2000_to_nzgd1949", linz_nzgd2000_nzgd1949, METH_VARARGS, linz_linz_nzgd2000_nzgd1949_docstring},
    {NULL, NULL, 0, NULL}
};

lnzGrid linz_grid;

PyMODINIT_FUNC init_linzgrid(void)
{
    PyObject *m = Py_InitModule3("_linzgrid", module_methods, module_docstring);
    if (m == NULL)
        return;

  linz_grid = *lnzGridNzgd2k();

    if (!lnzGridIsOk(&linz_grid)) {
        PyErr_SetString(PyExc_RuntimeError, lnzGridLastError(&linz_grid));
        lnzGridDestroy(&linz_grid);
        return;
    }
}

static PyObject *linz_nzgd2000_nzgd1949(PyObject *self, PyObject *args)
{
    double lt_2000, ln_2000, lt_1949, ln_1949; /* latitude (rad), longitude (rad) */
    PyObject ret;

    if (!PyArg_ParseTuple(args, "dd", &lt_2000, &ln_2000))
        return NULL;

    //TODO: test return code
    lnzGridTransform(&linz_grid, lt_1949, ln_1949, &lt_2000, &ln_2000, NZGD2000_TO_NZGD49);

    {
        PyObject *ret = Py_BuildValue("dd", lt_1949, ln_1949);
        return ret;
    }
}

static PyObject *linz_nzgd1949_nzgd2000(PyObject *self, PyObject *args)
{
    double lt_2000, ln_2000, lt_1949, ln_1949; /* latitude (rad), longitude (rad) */
    PyObject ret;

    if (!PyArg_ParseTuple(args, "dd", &lt_1949, &ln_1949))
        return NULL;

    //TODO: test return code
    lnzGridTransform(&linz_grid, lt_1949, ln_1949, &lt_2000, &ln_2000, NZGD49_TO_NZGD2000);

    {
        PyObject *ret = Py_BuildValue("dd", lt_2000, ln_2000);
        return ret;
    }
}
