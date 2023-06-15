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

void delete_vec(struct vector *head)
{
    struct vector *next;
    while (head->next != NULL)
    {
        next = head->next;
        delete_cord(head->cords);
        free(head);
        head = next;
    }
    free(head);
}

int file_err = 0;
struct vector *head_vec;
struct cord *head_cord;
int dim = 0;
double eps = 0.001;

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
            printf("An Error has Occurred\n");
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

void print_cent(struct vector cent[], int K)
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

struct vector *read_points(int d, int len, double points[d][len])
{
    struct vector *curr_vec;
    struct cord *curr_cord;
    int i;
    int j;

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

    for (i = 0; i < len; i++)
    {
        for (j = 0; j < d; j++)
        {
            curr_cord->value = points[i][j];
            curr_vec->cords = head_cord;
            curr_vec->next = malloc(sizeof(struct vector));
            if (curr_vec->next == NULL)
            {
                printf("An Error has Occurred\n");
                delete_vec(head_vec);
                exit(1);
            }
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            head_cord = malloc(sizeof(struct cord));
            if (head_cord == NULL)
            {
                printf("An Error has Occurred\n");
                delete_vec(head_vec);
                exit(1);
            }
            curr_cord = head_cord;
            curr_cord->next = NULL;
        }
    }
    delete_cord(curr_cord);
    return head_vec;
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
            printf("An Error has Occurred\n");
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

// check
void copy_init(struct vector *centroids, int vec_cords, double py_cent[][vec_cords], int K)
{
    int i;
    int j;
    struct vector new_vec;
    struct cord *new_cord;
    struct cord *prev_copy_cord = NULL;
    new_vec.cords = NULL;

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
        for (j = 0; j < vec_cords; j++)
        {
            new_cord = malloc(sizeof(struct cord));
            if (new_cord == NULL)
            {
                printf("An Error has Occurred\n");
                exit(1);
            }
            new_cord->value = py_cent[i][j];
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

struct vector *kmeans(int K, int iter, int vec_cords, int num_of_points, struct vector *init_cent, double points[][vec_cords])
{
    double temp_dist;
    int a = 0;
    int b = 0;
    int e = 0;
    int d = 0;
    int f = 0;
    int iter_err = 0;
    int K_err = 0;
    int N = num_of_points;

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
    centroids = init_cent;
    dim = vec_cords;

    if (iter <= 1 || iter >= 1000)
    {
        iter_err = 1;
        printf("%s", "Invalid maximum iteration!\n");
    }

    read_points(dim, num_of_points, points);

    if (K <= 1 || K >= N)
    {
        K_err = 1;
        printf("Invalid number of clusters!\n");
    }

    if (K_err || file_err || iter_err)
    {
        delete_vec(head_vec);
        exit(1);
    }

    temp_cent_arr = malloc(K * sizeof(struct vector));
    if (temp_cent_arr == NULL)
    {
        printf("An Error has Occurred\n");

        delete_vec(head_vec);
        exit(1);
    }

    while (counter < iter && cond)
    {
        points_in_temp = calloc(K, sizeof(int));
        if (points_in_temp == NULL)
        {
            printf("An Error has Occurred\n ");
            exit(1);
        }

        b = 0;
        for (; b < K; b++)
        {
            zero = create_zero_vec(dim);
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
            break;
        }
    }
    // print_cent(centroids, K);

    a = K - 1;
    for (; a >= 0; a--)
    {
        // delete_cord(centroids[a].cords);
        delete_cord(temp_cent_arr[a].cords);
    }
    // free(centroids);
    free(points_in_temp);
    free(temp_cent_arr);
    delete_vec(head_vec);
    return centroids;
}

// int main(int arvc, char *argv[])
// {
//     int K;
//     int iter_num;
//     if (arvc == 2)
//     {
//         K = atoi(argv[1]);
//         iter_num = 200;
//     }
//     else if (arvc == 3)
//     {
//         K = atoi(argv[1]);
//         iter_num = atoi(argv[2]);
//     }
//     else
//     {
//         printf("An Error has Occurred\n");
//         exit(1);
//     }

//     kmeans(K, iter_num, );
//     printf("\n");
//     return 1;
// }
