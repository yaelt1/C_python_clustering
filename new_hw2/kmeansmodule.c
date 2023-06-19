
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

void create_head_vec(struct vector *head_vec, PyObject *list, int n, int d)
{
    struct vector *cur_vec;
    struct cord *head_cord, cur_cord;

    head_vec = malloc(sizeof(struct vector));
    if (head_vec == NULL)
    {
        printf("An Error has Occurred\n");
        exit(1);
    }

    head_cord = malloc(sizeof(struct cord));
    if (head_cord == NULL)
    {
        printf("An Error has Occurred\n");
        exit(1);
    }
    curr_cord = head_cord;
    curr_cord->next = NULL;
    curr_vec = head_vec;
    curr_vec->next = NULL;

    for (int i = 0; i < n; i++)
    {
        PyObject *row = PyList_GetItem(lst, i);
        if (!PyList_Check(row))
        {
            printf("An Error has Occurred\n");
            return NULL;
        }

        for (int j = 0; j < d; j++)
        {
            PyObject *value = PyList_GetItem(row, j);
            curr_cord->value = PyFloat_AsDouble(value);
            curr_cord->next = malloc(sizeof(struct cord));
            if (curr_cord->next == NULL)
            {
                printf("An Error has Occurred\n");
                exit(1);
            }

            curr_cord = curr_cord->next;
            curr_cord->next = NULL;
        }

        // curr_cord->value = n;
        curr_vec->cords = head_cord;
        curr_vec->next = malloc(sizeof(struct vector));
        if (curr_vec->next == NULL)
        {
            printf("An Error has Occurred\n");
            exit(1);
        }
        curr_vec = curr_vec->next;
        curr_vec->next = NULL;
        head_cord = malloc(sizeof(struct cord));
        if (head_cord == NULL)
        {
            printf("An Error has Occurred\n");
            exit(1);
        }
        curr_cord = head_cord;
        curr_cord->next = NULL;
        continue;
    }

    delete_cord(curr_cord);
    return num;
}

void print_cent_mod(struct vector cent[], int K, int d)
{
    int i = 0;
    struct cord *cur;
    int range = d;
    for (; i < K; i++)
    {
        cur = cent[i].cords;
        range = d;
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

void delete_vec_mod(struct vector *head)
{
    struct vector *next;
    while (head->next != NULL)
    {
        next = head->next;
        delete_cord_mod(head->cords);
        free(head);
        head = next;
    }
}

static PyObject *fit(PyObject *self, PyObject *args)
{
    int K;
    int iter;
    int d = 0;
    PyObject *py_cent_obj;
    PyObject *lst;
    int a;
    int i;
    int j;
    int n;
    int eps;
    struct vector *head_vec = NULL;
    // struct vector *cur_vec = NULL;
    // struct cord *head_cord, cur_cord = NULL;

    struct vector *centroids = NULL;
    struct vector *final_cent = NULL;
    struct vector *new_vec = NULL;

    struct cord *new_cord;
    struct cord *prev_copy_cord, *to_list_cur_cord = NULL;
    PyObject *python_centroids;
    PyObject *python_vec;
    PyObject *python_float;
    if (!PyArg_ParseTuple(args, "iiiiOO", &K, &iter, &d, &eps, &py_cent_obj, &lst))
    {
        return NULL;
    }

    if (PyObject_Length(py_cent_obj) < 0)
    {
        return NULL;
    }

    n = PyObject_Length(lst) if (n < 0)
    {
        return NULL;
    }

    c centroids = malloc(K * sizeof(struct vector));
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
            printf("An Error has Occurred\n");
            return NULL;
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

    centroids = kmeans(K, iter, d, eps, centroids, string_of_points);
    // list of all centroids
    python_centroids = PyList_New(K);
    {
        python_vec = PyList_New(d);
        to_list_cur_cord = centroids[i].cords;
        for (j = 0; j < d; j++)
        {
            double value = to_list_cur_cord->value;
            python_float = Py_BuildValue("f", value);
            to_list_cur_cord = to_list_cur_cord->next;
            PyList_SetItem(python_vec, j, python_float);
        }
        PyList_SetItem(python_centroids, i, python_vec);
    }

    a = K - 1;
    for (; a >= 0; a--)
    {
        delete_cord_mod(centroids[a].cords);
    }
    free(centroids);

    return python_centroids;
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
};
