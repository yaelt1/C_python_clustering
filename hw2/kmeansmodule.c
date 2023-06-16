
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
void print_cent_mod(struct vector cent[], int K, int dim)
{
    int i = 0;
    struct cord *cur;
    int range;
    for (; i < K; i++)
    {
        cur = cent[i].cords;
        range = dim;
        while (range > 1)
        {
            printf("%.4f,", cur->value);
            cur = cur->next;
            range--;
        }
        printf("%.4f\n", cur->value);
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
    int a;
    int i;
    int j;
    int n = 0;
    int eps;
    struct vector *centroids = NULL;
    struct vector *final_cent = NULL;
    struct vector *new_vec = NULL;
    struct cord *new_cord;
    struct cord *prev_copy_cord = NULL;
    if (!PyArg_ParseTuple(args, "iiiiOO", &K, &iter, &d, &eps, &py_cent_obj, &lst))
    {
        return NULL;
    }

    if (PyObject_Length(py_cent_obj) < 0)
    {
        return NULL;
    }

    // // number of points
    n = PyObject_Length(lst);
    if (n < 0)
    {
        return NULL;
    }

    centroids = malloc(K * sizeof(struct vector));
    if (centroids == NULL)
    {
        printf("An Error has Occurred\n");
        return NULL;
    }
    for (i = 0; i < K; i++)
    {
        PyObject *row = PyList_GetItem(py_cent_obj, i);
        if (!PyList_Check(row))
        {
            // Handle error: the item is not a list
        }
        new_vec = malloc(sizeof(struct vector));
        if (new_vec == NULL)
        {
            printf("An Error has Occurred\n");
            return NULL;
        }

        new_vec->cords = NULL;
        new_cord = NULL;
        prev_copy_cord = NULL;

        for (j = 0; j < d; j++)
        {
            new_cord = malloc(sizeof(struct cord));
            if (new_cord == NULL)
            {
                printf("An Error has Occurred\n");
                return NULL;
            }
            PyObject *value = PyList_GetItem(row, j);
            new_cord->value = PyFloat_AsDouble(value);
            new_cord->next = NULL;
            if (prev_copy_cord != NULL)
            {
                prev_copy_cord->next = new_cord;
            }
            else
            {
                new_vec->cords = new_cord;
            }

            prev_copy_cord = new_cord;
        }

        centroids[i] = *new_vec;
    }

    kmeans(K, iter, d, eps, centroids);

    print_cent_mod(centroids, K, n);
    return PyLong_FromLong(90);

    // // create a python list
    // PyObject *python_list;
    // PyObject *python_vec;
    // PyObject *python_cord;
    // struct cord *cur_cord;
    // struct vector *vec;

    // python_list = PyList_New(K);

    // if (python_list == NULL)
    // {
    //     // Error handling
    //     delete_cord_mod(final_cent->cords);
    //     free(final_cent);
    //     return NULL;
    // }

    // for (i = 0; i < K; ++i)
    // {
    //     vec = &final_cent[i];
    //     cur_cord = vec->cords;
    //     python_vec = PyList_New(d);
    //     if (python_vec == NULL)
    //     {
    //         // Error handling
    //         delete_cord_mod(final_cent->cords);
    //         free(final_cent);
    //         Py_DECREF(python_list);
    //         return NULL;
    //     }

    //     for (j = 0; j < d; j++)
    //     {
    //         python_cord = PyFloat_FromDouble(cur_cord->value);
    //         PyList_SetItem(python_vec, j, python_cord);
    //         cur_cord = cur_cord->next;
    //     }
    //     PyList_SetItem(python_list, i, python_vec);
    // }

    // a = K - 1;
    // for (; a >= 0; a--)
    // {
    //     delete_cord_mod(centroids[a].cords);
    // }
    // free(centroids);

    // return python_list;
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