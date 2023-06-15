
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <math.h>
#include "cap.h"

struct cord
{
    double value;
    struct cord *next;
};

struct vector
{
    struct vector *next;
    struct cord *cords;
};

void copy_init_mod(struct vector *centroids, int dim, PyObject *py_cent_obj, int K)
{
    int i;
    int j;
    struct vector new_vec;
    struct cord *new_cord;
    struct cord *prev_copy_cord = NULL;
    new_vec.cords = NULL;
    PyObject *row;
    PyObject *value;

    centroids = malloc(K * sizeof(struct vector));
    if (centroids == NULL)
    {
        printf("An Error has Occurred\n");
        exit(1);
    }

    for (i = 0; i < K; i++)
    {
        new_cord = NULL;
        prev_copy_cord = NULL;
        row = PyList_GetItem(py_cent_obj, i);
        for (j = 0; j < dim; j++)
        {
            new_cord = malloc(sizeof(struct cord));
            if (new_cord == NULL)
            {
                printf("An Error has Occurred\n");
                exit(1);
            }
            value = PyList_GetItem(row, j);
            new_cord->value = PyFloat_AsDouble(value);
            new_cord->next = NULL;
            if (prev_copy_cord != NULL)
            {
                prev_copy_cord->next = new_cord;
            }
            else
            {
                new_vec.cords = new_cord;
            }

            prev_copy_cord = new_cord;
        }

        centroids[i] = new_vec;
    }
}

void delete_cord_mod(struct cord *first_cord)
{
    struct cord *cur;
    while (first_cord != NULL)
    {
        cur = first_cord->next;
        free(first_cord);
        first_cord = cur;
    }
}

static PyObject *fit(PyObject *self, PyObject *args)
{
    int K;
    int iter;
    int d = 0;
    PyObject *py_cent_obj;
    PyObject *lst;
    PyObject *py_vec;
    PyObject *cord;
    int i;
    int j;
    int a;
    int n = 0;
    double points[n][d];
    struct vector *centroids = NULL;
    struct vector *final_cent;

    if (!PyArg_ParseTuple(args, "iiiOO", &K, &iter, &d, &py_cent_obj, &lst))
    {
        return NULL;
    }
    int l = PyObject_Length(py_cent_obj);
    if (l < 0)
    {
        return NULL;
    }

    n = PyObject_Length(lst);
    if (n < 0)
    {
        return NULL;
    }

    copy_init_mod(centroids, d, py_cent_obj, K);
    for (i = 0; i < n; i++)
    {
        py_vec = PyList_GetItem(lst, i);
        for (j = 0; j < d; j++)
        {
            cord = PyList_GetItem(py_vec, j);
            points[i][j] = PyFloat_AsDouble(cord);
        }
    }

    final_cent = kmeans(K, iter, d, n, centroids, points);

    // create a python list
    PyObject *python_list;
    PyObject *python_vec;
    PyObject *python_cord;
    struct cord *cur_cord;
    struct vector *vec;

    python_list = PyList_New(K);

    if (python_list == NULL)
    {
        // Error handling
        delete_cord_mod(final_cent->cords);
        free(final_cent);
        return NULL;
    }

    for (i = 0; i < K; ++i)
    {
        vec = &final_cent[i];
        cur_cord = vec->cords;
        python_vec = PyList_New(d);
        if (python_vec == NULL)
        {
            // Error handling
            delete_cord_mod(final_cent->cords);
            free(final_cent);
            Py_DECREF(python_list);
            return NULL;
        }

        for (j = 0; j < d; j++)
        {
            python_cord = PyFloat_FromDouble(cur_cord->value);
            PyList_SetItem(python_vec, j, python_cord);
            cur_cord = cur_cord->next;
        }
        PyList_SetItem(python_list, i, python_vec);
    }

    a = K - 1;
    for (; a >= 0; a--)
    {
        delete_cord_mod(final_cent[a].cords);
    }
    free(final_cent);

    return python_list;
}

static PyMethodDef capiMethods[] = {
    {"fit",
     (PyCFunction)fit,
     METH_VARARGS,
     PyDoc_STR("kmeans func")},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",
    NULL,
    -1,
    capiMethods};

PyMODINIT_FUNC PyInit_mykmeanssp(void)
{
    PyObject *m;
    m = PyModule_Create(&moduledef);
    if (!m)
    {
        return NULL;
    }
    return m;
}