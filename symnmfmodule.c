
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <math.h>
#include "symnmf.h"

double **convertMatFromPy(PyObject *mat, int rows, int cols)
{
    int i, j;
    PyObject *row, *value;
    double **cMat = calloc(rows, sizeof(double *));
    if (cMat == NULL)
    {
        return NULL;
    }
    for (i = 0; i < rows; i++)
    {
        row = PyList_GetItem(mat, i);
        cMat[i] = calloc(cols, sizeof(double));
        if (cMat[i] == NULL)
        {
            return NULL;
        }
        for (j = 0; j < cols; j++)
        {
            value = PyList_GetItem(row, j);
            cMat[i][j] = PyFloat_AsDouble(value);
        }
    }
    return (cMat);
}

/*
fit function params:
*/
static PyObject *fit(PyObject *self, PyObject *args)
{
    int numOfPoints, dimentions, i, j, k;
    int indicator;
    PyObject *PyPoints, *row, *value, *py_mat_row, *py_mat, *python_float, *m;
    double avg;
    double **cPoints, **resultMat = NULL, **cH0, **cWMat;
    PyObject *py_H0 = Py_BuildValue("");
    PyObject *py_w_mat = Py_BuildValue("");

    if (!PyArg_ParseTuple(args, "iiiiO|OO", &indicator, &numOfPoints, &dimentions, &k, &PyPoints, &py_w_mat, &py_H0))
    {
        PyErr_SetString(PyExc_RuntimeError, "An error occurred during the operation");
        return NULL;
    }
    cPoints = calloc(numOfPoints, sizeof(double *));
    if (cPoints == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "An error occurred during the operation");
        return NULL;
    }

    for (i = 0; i < numOfPoints; i++)
    {
        row = PyList_GetItem(PyPoints, i);
        if (!PyList_Check(row))
        {
            PyErr_SetString(PyExc_RuntimeError, "An error occurred during the operation");
            return NULL;
        }

        cPoints[i] = calloc(dimentions, sizeof(double));
        if (cPoints[i] == NULL)
        {
            PyErr_SetString(PyExc_RuntimeError, "An error occurred during the operation");
            return NULL;
        }

        for (j = 0; j < dimentions; j++)
        {
            value = PyList_GetItem(row, j);
            double val = PyFloat_AsDouble(value);
            cPoints[i][j] = val;
        }
    }

    updateGlobals(numOfPoints, dimentions, k);
    // get m from C to init H0
    if (indicator == 0)
    {
        avg = avgToPy(cPoints);
        m = Py_BuildValue("f", avg);
        return m;
    }

    if (indicator == 1)
    {

        resultMat = sym(cPoints);
    }

    if (indicator == 2)
    {
        resultMat = ddg(cPoints);
    }
    if (indicator == 3)
    {
        resultMat = norm(cPoints);
    }
    if (indicator == 4)
    {
        cWMat = convertMatFromPy(py_w_mat, numOfPoints, numOfPoints);
        if (cWMat == NULL)
        {
            PyErr_SetString(PyExc_RuntimeError, "An error occurred during the operation");
            return NULL;
        }
        cH0 = convertMatFromPy(py_H0, numOfPoints, k);
        if (cH0 == NULL)
        {
            PyErr_SetString(PyExc_RuntimeError, "An error occurred during the operation");
            return NULL;
        }
        resultMat = symnmf(cH0, cWMat);
        if (resultMat == NULL)
        {
            PyErr_SetString(PyExc_RuntimeError, "An error occurred during the operation");
            return NULL;
        }
        py_mat = PyList_New(numOfPoints);
        for (i = 0; i < numOfPoints; i++)
        {
            py_mat_row = PyList_New(k);
            for (j = 0; j < k; j++)
            {
                double value = resultMat[i][j];
                python_float = Py_BuildValue("f", value);
                PyList_SetItem(py_mat_row, j, python_float);
            }
            PyList_SetItem(py_mat, i, py_mat_row);
        }
        return py_mat;
    }

    if (resultMat == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "An error occurred during the operation");
        return NULL;
    }
    py_mat = PyList_New(numOfPoints);
    for (i = 0; i < numOfPoints; i++)
    {
        py_mat_row = PyList_New(numOfPoints);
        for (j = 0; j < numOfPoints; j++)
        {
            double value = resultMat[i][j];
            python_float = Py_BuildValue("f", value);
            PyList_SetItem(py_mat_row, j, python_float);
        }
        PyList_SetItem(py_mat, i, py_mat_row);
    }

    return py_mat;
}

static PyMethodDef capiMethods[] = {
    {"fit",
     (PyCFunction)fit,
     METH_VARARGS,
     PyDoc_STR("")},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "symnmf",
    NULL,
    -1,
    capiMethods};

PyMODINIT_FUNC PyInit_symnmf(void)
{
    PyObject *a;
    a = PyModule_Create(&moduledef);
    if (!a)
    {
        return (NULL);
    }
    return a;
};
