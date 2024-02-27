#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PY_SIZE_T_CLEAN
#include <Python.h>

typedef struct
{
    int size;
    double *center;
    double *currentCenter;
} Centroid;

int findClosestCluster(double[], Centroid[], int, int);
double CalcEclideanDistance(double[], double[], int);
int isNumber(char[]);
int kmeans(int, int, int, int, double, Centroid[], double *[]);

double CalcEclideanDistance(double u[], double v[], int d)
{
    double squareSum;
    int i;
    squareSum = 0;
    for (i = 0; i < d; i++)
    {
        squareSum += (u[i] - v[i]) * (u[i] - v[i]);
    }

    return sqrt(squareSum);
}

int findClosestCluster(double dataPoint[], Centroid centroids[], int d, int k)
{
    int closest, i;
    double minDistance, distance;
    closest = 0;
    minDistance = CalcEclideanDistance(dataPoint, centroids[0].center, d);

    for (i = 0; i < k; i++)
    {
        distance = CalcEclideanDistance(dataPoint, centroids[i].center, d);
        if (distance < minDistance)
        {
            minDistance = distance;
            closest = i;
        }
    }
    return closest;
}

int isNumber(char number[])
{
    int i;
    i = 0;

    if (number[0] == '-')
        i = 1;
    for (; number[i] != 0; i++)
    {

        if (number[i] > '9' || number[i] < '0')
            return 0;
    }
    return 1;
}

int kmeans(int k, int n, int d, int iter, double e, Centroid centroids[], double *dataPoints[])
{
    int i, j, l, closest;
    double maxDelta, delta;

    /* main loop */
    maxDelta = 1 + e;
    for (i = 0; (i < iter) && (maxDelta > e); i++)
    {
        /* find closest cluster for every x in dataPoints */
        for (j = 0; j < n; j++)
        {
            closest = findClosestCluster(dataPoints[j], centroids, d, k);
            centroids[closest].size += 1;
            for (l = 0; l < d; l++)
            {
                centroids[closest].currentCenter[l] = centroids[closest].currentCenter[l] + dataPoints[j][l];
            }
        }
        /* recenter all clusters and update maxDelta */
        maxDelta = 0;
        for (j = 0; j < k; j++)
        {
            for (l = 0; l < d; l++)
            {
                centroids[j].currentCenter[l] = (centroids[j].currentCenter[l]) / (centroids[j].size);
            }
            delta = CalcEclideanDistance(centroids[j].currentCenter, centroids[j].center, d);
            if (delta > maxDelta)
            {
                maxDelta = delta;
            }

            for (l = 0; l < d; l++)
            {
                centroids[j].center[l] = centroids[j].currentCenter[l];
                centroids[j].currentCenter[l] = 0;
                centroids[j].size = 0;
            }
        }
    }

    return 0;
}

static PyObject *find_kmeans(PyObject *self, PyObject *args)
{
    PyObject *centroidsLst, *datapointsLst;
    PyObject *row;
    PyObject *item;
    int n, k, d, iter;
    double e;
    int returnValue;
    double **dataPoints;
    Centroid *centroids;
    returnValue = 0;

    if (!PyArg_ParseTuple(args, "OOid", &centroidsLst, &datapointsLst, &iter, &e))
    {
        return NULL;
    }

    n = PyObject_Length(datapointsLst);
    k = PyObject_Length(centroidsLst);
    d = PyObject_Length(PyList_GetItem(datapointsLst, 0));

    centroids = calloc(k, sizeof(Centroid));
    dataPoints = (double **)calloc(n, sizeof(double *));

    if (centroids == NULL || dataPoints == NULL)
    {
        printf("An Error Has Occurred");
        returnValue = 1;
        goto FREE;
    }

    for (int i = 0; i < n; i++)
    {
        row = PyList_GetItem(datapointsLst, i);
        dataPoints[i] = (double *)calloc(d, sizeof(double));
        if (dataPoints[i] == NULL)
        {
            printf("An Error Has Occurred");
            returnValue = 1;
            goto FREE;
        }
        for (int j = 0; j < d; j++)
        {
            item = PyList_GetItem(row, j);
            dataPoints[i][j] = PyFloat_AsDouble(item);
        }
    }
    for (int i = 0; i < k; i++)
    {
        row = PyList_GetItem(centroidsLst, i);
        centroids[i].size = 0;
        centroids[i].center = (double *)calloc(d, sizeof(double));
        centroids[i].currentCenter = (double *)calloc(d, sizeof(double));
        if (centroids[i].center == NULL || centroids[i].currentCenter == NULL)
        {
            printf("An Error Has Occurred");
            returnValue = 1;
            goto FREE;
        }
        for (int j = 0; j < d; j++)
        {
            item = PyList_GetItem(row, j);
            centroids[i].center[j] = PyFloat_AsDouble(item);
            centroids[i].currentCenter[j] = 0;
        }
    }

    kmeans(k, n, d, iter, e, centroids, dataPoints);

    centroidsLst = PyList_New(k);

    for (int i = 0; i < k; i++)
    {
        row = PyList_New(d);
        for (int j = 0; j < d; j++)
        {
            item = Py_BuildValue("d", centroids[i].center[j]);
            PyList_SetItem(row, j, item);
        }
        PyList_SetItem(centroidsLst, i, row);
    }
FREE:
    for (int i = 0; i < k; i++)
    {
        free(centroids[i].center);
        free(centroids[i].currentCenter);
    }
    free(centroids);

    for (int i = 0; i < n; i++)
    {
        free(dataPoints[i]);
    }
    free(dataPoints);

    return centroidsLst;
}

static PyMethodDef kmeansMethods[] = {
    {"fit",
     (PyCFunction)find_kmeans,
     METH_VARARGS,
     PyDoc_STR("find k means for set of datapoints. pass- iter: maximum iterations, e: accaptable accuracy for the means. centroids: array of initialized centroids, dataPoints: array of datapoints ")},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef kmeansmodule = {
    PyModuleDef_HEAD_INIT,
    "kmeans",
    NULL,
    -1,
    kmeansMethods};

PyMODINIT_FUNC PyInit_kmeans(void)
{
    PyObject *m;
    m = PyModule_Create(&kmeansmodule);
    if (!m)
    {
        return NULL;
    }
    return m;
}