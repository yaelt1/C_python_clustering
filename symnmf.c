#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int n = 0;
int dim = 0;
double eps = 0.0001;
int max_iter = 300;
int clustersNum = 0;

void printMat(double **mat, int rows, int cols)
{
    int i, j;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols - 1; j++)
        {
            printf("%.4f,", mat[i][j]);
        }
        printf("%.4f\n", mat[i][j]);
    }
}

void findDims(char *path)
{
    FILE *file;
    int pointsCount = 0;
    int dimensionCount = 0;
    char c;
    file = fopen(path, "r");
    if (file == NULL)
    {
        perror("An Error has Occurred\n");
        return;
    }
    while ((fscanf(file, "%c", &c) == 1))
    {
        if (c == '\n')
        {
            pointsCount++;
            if (dim == 0)
            {
                dim = dimensionCount + 1;
            }
            dimensionCount = 0;
        }
        else
        {
            dimensionCount++;
        }
    }
    if (c != '\n')
    {
        n = pointsCount + 1;
    }
    else
    {
        n = pointsCount;
    }
    fclose(file);
}

void updateGlobals(int num, int d, int k)
{
    n = num;
    dim = d;
    clustersNum = k;
}

double **readFile(char *path)
{
    double value;
    int i, j;
    FILE *file;
    double **points = calloc(n, sizeof(double *));
    if (points == NULL)
    {
        printf("An Error has Occurred\n");
        exit(1);
    }

    file = fopen(path, "r");
    if (file == NULL)
    {
        perror("An Error has Occurred\n");
        return NULL;
    }
    for (i = 0; i < n; i++)
    {
        points[i] = calloc(dim, sizeof(double));

        if (points[i] == NULL)
        {
            printf("An Error has Occurred\n");
            exit(1);
        }

        for (j = 0; j < dim; j++)
        {
            if (fscanf(file, "%lf", &value) == 1)
            {
                points[i][j] = value;
            }
        }
    }
    fclose(file);
    return points;
}

void freeMat(double **mat, int rows)
{
    int i;
    for (i = 0; i < rows; i++)
    {
        free(mat[i]);
    }
    free(mat);
}

double calcOcDist(double *x1, double *x2)
{
    double res = 0.0;
    double sum;
    int i = 0;
    for (; i < dim; i++)
    {
        sum = (x1[i]) - (x2[i]);
        res += pow(sum, 2);
    }
    return (res);
}

double **sym(double **X)
{
    int i, j;
    double val;
    double **symMat;
    symMat = calloc(n, sizeof(double *));
    if (symMat == NULL)
    {
        return NULL;
    }

    for (i = 0; i < n; i++)
    {
        symMat[i] = calloc(n, sizeof(double));
        if (symMat[i] == NULL)
        {
            return NULL;
        }
        for (j = 0; j < n; j++)
        {
            if (j != i)
            {
                val = exp(-0.5 * calcOcDist(X[i], X[j]));
                symMat[i][j] = val;
            }
        }
    }
    return symMat;
}

double **ddg(double **X)
{
    int i, j;
    double sum = 0;
    double **symMat = sym(X);
    double **ddgMat = calloc(n, sizeof(double *));
    if (ddgMat == NULL)
    {
        return NULL;
    }

    for (i = 0; i < n; i++)
    {
        ddgMat[i] = calloc(n, sizeof(double));
        if (ddgMat[i] == NULL)
        {
            return NULL;
        }
        for (j = 0; j < n; j++)
        {
            sum += symMat[i][j];
        }
        ddgMat[i][i] = sum;
        sum = 0;
    }
    freeMat(symMat, n);
    return ddgMat;
}

double **matrixMultiply(double **firstMat, double **secondMat, int firstMatRows, int firstColSecRow, int secMatCols)
{
    int i, j, b;
    double **result = calloc(firstMatRows, sizeof(double *));
    if (result == NULL)
    {
        return NULL;
    }
    for (i = 0; i < firstMatRows; i++)
    {
        result[i] = calloc(secMatCols, sizeof(double));
        if (result[i] == NULL)
        {
            return NULL;
        }
        for (j = 0; j < secMatCols; j++)
        {
            for (b = 0; b < firstColSecRow; b++)
            {
                result[i][j] += firstMat[i][b] * secondMat[b][j];
            }
        }
    }
    return result;
}

double **sqrtMat(double **matrix)
{
    int i;
    double **sqrtMat = calloc(n, sizeof(double *));
    if (sqrtMat == NULL)
    {
        return NULL;
    }
    for (i = 0; i < n; i++)
    {
        sqrtMat[i] = calloc(n, sizeof(double));
        if (sqrtMat[i] == NULL)
        {
            return NULL;
        }
        if (matrix[i][i] != 0)
        {
            sqrtMat[i][i] = 1 / sqrt(matrix[i][i]);
        }
    }
    return sqrtMat;
}

double **norm(double **X)
{
    double **ddgMat = ddg(X);
    double **symMat = sym(X);
    double **sqrtD = sqrtMat(ddgMat);
    double **firstMul = matrixMultiply(sqrtD, symMat, n, n, n);
    double **norm = matrixMultiply(firstMul, sqrtD, n, n, n);
    freeMat(symMat, n);
    freeMat(ddgMat, n);
    freeMat(sqrtD, n);
    freeMat(firstMul, n);
    return norm;
}

double **transposeMat(double **matrix)
{
    int j, i;
    double **transpose = calloc(clustersNum, sizeof(double *));

    for (i = 0; i < clustersNum; i++)
    {
        transpose[i] = calloc(n, sizeof(double));
        if (transpose[i] == NULL)
        {
            return NULL;
        }
        for (j = 0; j < n; j++)
        {
            transpose[i][j] = matrix[j][i];
        }
    }
    return transpose;
}

double **subMat(double **firstMat, double **secondMat)
{
    int i, j;
    double **result = calloc(n, sizeof(double *));
    if (result == NULL)
    {
        return NULL;
    }
    for (i = 0; i < n; i++)
    {
        result[i] = calloc(clustersNum, sizeof(double));
        if (result[i] == NULL)
        {
            return NULL;
        }
        for (j = 0; j < clustersNum; j++)
        {
            result[i][j] += firstMat[i][j] - secondMat[i][j];
        }
    }
    return result;
}

double Fnorm(double **mat)
{
    double result = 0;
    int j, i;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < clustersNum; j++)
        {
            result += pow(mat[i][j], 2);
        }
    }
    return result;
}

double **updateH(double **H, double **W)
{
    int i, j, rounds = 0;
    double b = 0.5;
    double res;
    double **WH, **tranposeH, **HHt, **HHtH, **newH, **sub;

    while (rounds < max_iter)
    {
        WH = matrixMultiply(W, H, n, n, clustersNum);
        tranposeH = transposeMat(H);

        HHt = matrixMultiply(H, tranposeH, n, clustersNum, n);

        HHtH = matrixMultiply(HHt, H, n, n, clustersNum);

        newH = calloc(n, sizeof(double *));
        if (newH == NULL)
        {
            return NULL;
        }
        sub = calloc(n, sizeof(double *));
        if (sub == NULL)
        {
            return NULL;
        }
        for (i = 0; i < n; i++)
        {
            newH[i] = calloc(clustersNum, sizeof(double));
            if (newH[i] == NULL)
            {
                return NULL;
            }
            for (j = 0; j < clustersNum; j++)
            {
                newH[i][j] = H[i][j] * (1 - b + (b * (WH[i][j] / (HHtH[i][j]))));
            }
        }

        sub = subMat(newH, H);
        res = Fnorm(sub);
        freeMat(H, n);
        H = newH;
        freeMat(WH, n);
        freeMat(tranposeH, clustersNum);
        freeMat(HHt, n);
        freeMat(HHtH, n);
        freeMat(sub, n);
        if (res >= eps)
        {
            rounds++;
        }
        else
        {
            break;
        }
    }
    return H;
}

double avgToPy(double **X)
{
    int i, j;
    double result, sum = 0;
    double **W = norm(X);
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            sum += W[i][j];
        }
    }
    freeMat(W, n);
    result = sum / pow(n, 2);
    return (result);
}

double **symnmf(double **H0, double **W)

{
    double **resH = updateH(H0, W);
    return resH;
}

int main(int arvc, char *argv[])
{
    double **points, **result = NULL;
    char *goal, *filename;
    if (arvc != 3)
    {
        return 1;
    }
    goal = argv[1];
    filename = argv[2];
    findDims(filename);
    points = readFile(filename);

    if (strcmp(goal, "sym") == 0)
    {
        result = sym(points);
    }
    if (strcmp(goal, "ddg") == 0)
    {
        result = ddg(points);
    }
    if (strcmp(goal, "norm") == 0)
    {
        result = norm(points);
    }
    if (result != NULL)
    {
        printMat(result, n, n);
        freeMat(result, n);
        return 0;
    }
    else
    {
        return 1;
    }
}
