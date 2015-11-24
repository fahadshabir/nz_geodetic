#include <Python.h>
#include <linzgrid.h>
#include <linzgridnzgd2k.h>
#include <nzmg.h>
#include <nztm.h>

#define NZGD49_TO_NZGD2000 0
#define NZGD2000_TO_NZGD49 1

static char module_docstring[] =
  "A module for converting between New Zealand Map Grid and NZTM2000.";

static PyObject *linz_nzmg_to_nztm(PyObject *self, PyObject *args);
static char linz_nzmg_to_nztm_docstring[] =
  "Convert a point from New Zealand Map Grid to NZTM2000.\
  \n\n\
  nzmg_to_nztm2000(easting: float, northing: float) -> Tuple[float, float]";

static PyObject *linz_nztm_to_nzmg(PyObject *self, PyObject *args);
static char linz_nztm_to_nzmg_docstring[] =
    "Convert a point from NZTM2000 to New Zealand Map Grid.\
    \n\n\
    nztm2000_to_nzmg(easting: float, northing: float) -> Tuple[float, float]";

static PyMethodDef module_methods[] = {
    {"nzmg_to_nztm2000", linz_nzmg_to_nztm, METH_VARARGS, linz_nzmg_to_nztm_docstring},
    {"nztm2000_to_nzmg", linz_nztm_to_nzmg, METH_VARARGS, linz_nztm_to_nzmg_docstring},
    {NULL, NULL, 0, NULL}
};

lnzGrid linz_grid;

PyMODINIT_FUNC init_nz_geodetic(void)
{
    PyObject *m = Py_InitModule3("_nz_geodetic", module_methods, module_docstring);
    if (m == NULL)
        return;

    linz_grid = *lnzGridNzgd2k();

    if (!lnzGridIsOk(&linz_grid)) {
        PyErr_SetString(PyExc_RuntimeError, lnzGridLastError(&linz_grid));
        lnzGridDestroy(&linz_grid);
        return;
    }
}

static PyObject *linz_nzmg_to_nztm(PyObject *self, PyObject *args)
{
    double n, e, lt49, ln49, lt2000, ln2000, n2000, e2000;
    PyObject ret;

    if (!PyArg_ParseTuple(args, "dd", &e, &n))
        return NULL;

    nzmg_geod(n, e, &lt49, &ln49);
    lnzGridTransform(&linz_grid, lt49, ln49, &lt2000, &ln2000, NZGD49_TO_NZGD2000); //TODO: test return code
    geod_nztm(lt2000, ln2000, &n2000, &e2000);

    {
        PyObject *ret = Py_BuildValue("dd", e2000, n2000);
        return ret;
    }
}

static PyObject *linz_nztm_to_nzmg(PyObject *self, PyObject *args)
{
    double n49, e49, lt49, ln49, lt2000, ln2000, n2000, e2000;
    PyObject ret;

    if (!PyArg_ParseTuple(args, "dd", &e2000, &n2000))
        return NULL;

    nzmg_geod(n2000, e2000, &lt2000, &ln2000);
    lnzGridTransform(&linz_grid, lt2000, ln2000, &lt49, &ln49, NZGD2000_TO_NZGD49); //TODO: test return code
    geod_nztm(lt49, ln49, &n49, &e49);

    {
        PyObject *ret = Py_BuildValue("dd", e49, n49);
        return ret;
    }
}
