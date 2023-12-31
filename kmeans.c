#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cap.h"

#define gnu source

#include <limits.h>

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

void delete_cord(struct cord *first_cord)
{
    struct cord *cur;
    while (first_cord != NULL)
    {
        cur = first_cord->next;
        free(first_cord);
        first_cord = cur;
    }
}

struct vector *head_vec;
struct cord *head_cord;
int dim = 0;
double eps = 0;

struct vector create_zero_vec(int dim)
{
    struct vector new_vec;
    struct cord *copy_cord;
    int range = 0;
    struct cord *prev_copy_cord = NULL;
    new_vec.cords = NULL;

    while (range < dim)
    {
        range++;
        copy_cord = malloc(sizeof(struct cord));
        if (copy_cord == NULL)
        {

            exit(1);
        }
        copy_cord->value = 0.0;
        copy_cord->next = NULL;
        if (prev_copy_cord != NULL)
        {
            prev_copy_cord->next = copy_cord;
        }
        else
        {
            new_vec.cords = copy_cord;
        }
        prev_copy_cord = copy_cord;
    }
    return new_vec;
}

double cal_dist(struct vector *x1, struct vector *x2)
{
    double res = 0.0;
    struct cord *cur_x1 = x1->cords;
    struct cord *cur_x2 = x2->cords;
    int range = dim;
    double sum;
    while (range > 0)
    {
        sum = (cur_x1->value) - (cur_x2->value);
        res += pow(sum, 2);
        cur_x1 = cur_x1->next;
        cur_x2 = cur_x2->next;
        range--;
    }
    return sqrt(res);
}

void add_to_temp(struct vector *x1, struct vector arr[], int index)
{
    struct cord *cur_x1 = x1->cords;
    struct cord *cur = arr[index].cords;
    int range = dim;
    while (range > 0)
    {
        cur->value += cur_x1->value;
        cur_x1 = cur_x1->next;
        cur = cur->next;
        range--;
    }
}

void cal_avg(struct vector arr[], const int counter[], int K)
{
    int i = 0;
    struct cord *cur;
    int len;
    int range;
    for (; i < K; i++)
    {
        if (counter[i] != 0)
        {
            len = counter[i];
            cur = arr[i].cords;
            range = dim;
            while (range > 0)
            {
                cur->value = ((cur->value) / len);
                cur = cur->next;
                range--;
            }
        }
    }
}

struct vector copy_vec(struct vector *vec)
{
    struct vector new_vec;
    struct cord *org_cord = vec->cords;
    struct cord *copy_cord;
    struct cord *prev_copy_cord = NULL;
    new_vec.cords = NULL;

    while (org_cord != NULL)
    {
        copy_cord = malloc(sizeof(struct cord));
        if (copy_cord == NULL)
        {
            exit(1);
        }

        copy_cord->value = org_cord->value;
        copy_cord->next = NULL;

        if (prev_copy_cord != NULL)
        {
            prev_copy_cord->next = copy_cord;
        }
        else
        {
            new_vec.cords = copy_cord;
        }
        prev_copy_cord = copy_cord;
        org_cord = org_cord->next;
    }
    return new_vec;
}

void copy_cent(struct vector *centroid, struct vector *org_cent, int K)
{
    int i;
    for (i = 0; i < K; i++)
    {
        centroid[i] = copy_vec(&org_cent[i]);
        if (centroid[i].cords == NULL)
        {
            exit(1);
        }
    }
}
struct vector *kmeans(int K, int iter, int vec_cords, double epsilon, int len, struct vector *init_cent, struct vector *init_head_vec)
{
    double temp_dist;
    int a = 0;
    int b = 0;
    int e = 0;
    int d = 0;
    int f = 0;

    int N = len;
    struct vector *centroids = NULL;
    int counter = 0;
    int *points_in_temp;
    double min_dist = INT_MAX;
    double cur;
    int min_index_im_cent;
    struct vector *points_vec;
    struct vector *temp_cent_arr;
    double max_dist = INT_MIN;
    struct vector zero;
    int num_of_vec = 0;
    int cond = 1;
    eps = epsilon;
    dim = vec_cords;
    head_vec = init_head_vec;

    centroids = malloc(K * sizeof(struct vector));
    if (centroids == NULL)
    {

        return NULL;
    }
    copy_cent(centroids, init_cent, K);

    temp_cent_arr = malloc(K * sizeof(struct vector));
    if (temp_cent_arr == NULL)
    {
        return NULL;
    }

    while (counter < iter && cond)
    {
        points_in_temp = calloc(K, sizeof(int));
        if (points_in_temp == NULL)
        {
            return NULL;
        }

        b = 0;
        for (; b < K; b++)
        {
            zero = create_zero_vec(dim);
            if (zero.cords == NULL)
            {
                return NULL;
            }
            temp_cent_arr[b] = zero;
        }
        points_vec = head_vec;
        num_of_vec = 0;
        while (num_of_vec < N)
        {
            e = 0;
            min_dist = INT_MAX;
            for (; e < K; e++)
            {
                cur = cal_dist(&centroids[e], points_vec);
                if (cur < min_dist)
                {
                    min_dist = cur;
                    min_index_im_cent = e;
                }
            }
            add_to_temp(points_vec, temp_cent_arr, min_index_im_cent);
            points_in_temp[min_index_im_cent] += 1;
            points_vec = points_vec->next;
            num_of_vec++;
        }
        counter++;
        cal_avg(temp_cent_arr, points_in_temp, K);
        d = 0;
        max_dist = INT_MIN;
        for (; d < K; d++)
        {
            temp_dist = cal_dist(&centroids[d], &temp_cent_arr[d]);
            if (temp_dist > max_dist)
            {
                max_dist = temp_dist;
            }
        }

        if (max_dist >= eps)
        {
            f = K - 1;
            for (; f >= 0; f--)
            {
                delete_cord(centroids[f].cords);
                centroids[f] = copy_vec(&temp_cent_arr[f]);
                delete_cord(temp_cent_arr[f].cords);
            }
            free(points_in_temp);
        }
        else
        {
            cond = 0;
            a = K - 1;
            for (; a >= 0; a--)
            {
                delete_cord(temp_cent_arr[a].cords);
            }
            free(temp_cent_arr);
            free(points_in_temp);
            break;
        }
    }
    return (centroids);
}
