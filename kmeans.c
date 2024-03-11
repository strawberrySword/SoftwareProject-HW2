#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPSILON 0.001

typedef struct
{
   int size;
   double *center;
   double *currentCenter;
} Centroid;

int findClosestCluster(double[], Centroid[], int, int);
double CalcEclideanDistance(double[], double[], int);
int isNumber(char[]);
int kmeans(int, int, int, int, Centroid[], double *[]);

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

int kmeans(int k, int n, int iter, int d, Centroid centroids[], double *dataPoints[])
{
   int i, j, l, closest;
   double maxDelta, delta;

   /* main loop */
   maxDelta = 1 + EPSILON;
   for (i = 0; (i < iter) && (maxDelta > EPSILON); i++)
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

   /* print out the centrouds */
   for (i = 0; i < k; i++)
   {
      for (j = 0; j < d; j++)
      {
         if (j == d - 1)
         {
            printf("%.4f", centroids[i].center[j]);
         }
         else
         {
            printf("%.4f,", centroids[i].center[j]);
         }
      }
      printf("\n");
   }
   return 0;
}

int main(int argc, char *argv[])
{
   /* declerations */
   int i, j, n, k, d, iter, returnValue;
   double current;
   Centroid *centroids;
   double **dataPoints;

   /* parsing parameters */
   if (!(argc == 4 || argc == 5))
   {
      printf("An Error Has Occurred");
      return 1;
   }
   returnValue = 0;

   k = atoi(argv[1]);
   n = atoi(argv[2]);
   d = atoi(argv[3]);
   iter = 200;
   if (argc == 5)
   {
      iter = atoi(argv[4]);
      if ((iter <= 0 || iter >= 1000) || !isNumber(argv[4]))
      {
         printf("Invalid maximum iteration!");
         return 1;
      }
   }
   if (n <= 1 || !isNumber(argv[2]))
   {
      printf("Invalid number of points!");
      return 1;
   }
   if ((k <= 1 || k >= n) || !isNumber(argv[1]))
   {
      printf("Invalid number of clusters!");
      return 1;
   }
   if (d <= 0 || !isNumber(argv[3]))
   {
      printf("Invalid dimension of point!");
      return 1;
   }

   centroids = calloc(k, sizeof(Centroid));
   dataPoints = (double **)calloc(n, sizeof(double *));

   if (centroids == NULL || dataPoints == NULL)
   {
      printf("An Error Has Occurred");
      returnValue = 1;
      goto FREE;
   }

   for (i = 0; i < n; i++)
   {
      dataPoints[i] = (double *)calloc(d, sizeof(double));
      if (dataPoints[i] == NULL)
      {
         printf("An Error Has Occurred");
         returnValue = 1;
         goto FREE;
      }
   }

   /* parsing the input */
   for (i = 0; i < n; i++)
   {
      for (j = 0; (j < d) && (scanf("%lf,", &current) != EOF); j++)
      {
         dataPoints[i][j] = current;
      }
   }

   /* initialization and centroids allocation */
   for (i = 0; i < k; i++)
   {
      centroids[i].size = 0;
      centroids[i].center = (double *)calloc(d, sizeof(double));
      centroids[i].currentCenter = (double *)calloc(d, sizeof(double));
      if (centroids[i].center == NULL || centroids[i].currentCenter == NULL)
      {
         printf("An Error Has Occurred");
         returnValue = 1;
         goto FREE;
      }

      for (j = 0; j < d; j++)
      {
         centroids[i].center[j] = dataPoints[i][j];
         centroids[i].currentCenter[j] = 0;
      }
   }

   kmeans(k, n, iter, d, centroids, dataPoints);

   /* free all allocated space */
FREE:
   for (i = 0; i < k; i++)
   {
      free(centroids[i].center);
      free(centroids[i].currentCenter);
   }
   free(centroids);

   for (i = 0; i < n; i++)
   {
      free(dataPoints[i]);
   }
   free(dataPoints);
   return returnValue;
}