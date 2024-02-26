#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define EPSILON 0.001

typedef struct
{
   int size;
   double *center;
   double *currentCenter;
} Centroid;
typedef struct
{
   double *point;
   double distance;
} Point;

int findClosestCluster(double[], Centroid[], int, int);
double CalcEclideanDistance(double[], double[], int);
int isNumber(char[]);
double findClosestCenter(double dataPoint[], Centroid centroids[], int d, int k);
int checkIfInCenters(double dataPoint[], Centroid centroids[], int d, int k);
void merge(Point arr[], int l, int m, int r);
void mergeSort(Point arr[], int l, int r);

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
double findClosestCenter(double dataPoint[], Centroid centroids[], int d, int k)
{
   int i;
   double minDistance, distance;
   /* Checking if the dataPoint is one of the centers already*/
   if (checkIfInCenters(dataPoint, centroids, d, k) == 1)
   {
      return 0;
   }
   minDistance = CalcEclideanDistance(dataPoint, centroids[0].center, d);

   for (i = 1; i < k; i++)
   {
      distance = CalcEclideanDistance(dataPoint, centroids[i].center, d);
      if (distance < minDistance)
      {
         minDistance = distance;
      }
   }
   return minDistance;
}

/* Checking if the dataPoint is one of the centers already*/

int checkIfInCenters(double dataPoint[], Centroid centroids[], int d, int k)
{
   int i, j, check;
   check = 1;
   for (i = 0; i < k; i++)
   {
      for (j = 0; j < d; j++)
      {
         if (dataPoint[j] == centroids[i].center[j])
         {
            check = 1;
         }
         else
         {
            check = 0;
            break;
         }
      }
   }
   return check;
}

/*------ merge point------*/
void merge(Point arr[], int l, int m, int r)
{
   int i, j, k;
   int n1 = m - l + 1;
   int n2 = r - m;
   Point *L, *R;

   L = calloc(n1, sizeof(Point));
   R = calloc(n2, sizeof(Point));

   for (i = 0; i < n1; i++)
      L[i] = arr[l + i];
   for (j = 0; j < n2; j++)
      R[j] = arr[m + 1 + j];

   i = 0;
   j = 0;
   k = l;
   while (i < n1 && j < n2)
   {
      if (L[i].distance <= R[j].distance)
      {
         arr[k].distance = L[i].distance;
         i++;
      }
      else
      {
         arr[k].distance = R[j].distance;
         j++;
      }
      k++;
   }

   while (i < n1)
   {
      arr[k].distance = L[i].distance;
      i++;
      k++;
   }

   while (j < n2)
   {
      arr[k].distance = R[j].distance;
      j++;
      k++;
   }
   free(L);
   free(R);
}

void mergeSort(Point arr[], int l, int r)
{
   if (l < r)
   {

      int m = l + (r - l) / 2;

      mergeSort(arr, l, m);
      mergeSort(arr, m + 1, r);

      merge(arr, l, m, r);
   }
}
/*------ end------*/

int main(int argc, char *argv[])
{
   /* declerations */
   int i, j, l, n, k, d, iter, closest, returnValue, first, p, here, random;
   double current, maxDelta, delta, alldistance, randomNumber;
   Centroid *centroids;
   Point *points;
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

   /* allocation */

   centroids = calloc(k, sizeof(Centroid));
   points = calloc(n, sizeof(Point));
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
   /* initialization and Points allocation */
   for (i = 0; i < n; i++)
   {
      points[i].distance = 0;
      points[i].point = (double *)calloc(d, sizeof(double));
      for (j = 0; j < d; j++)
      {
         points[i].point[j] = dataPoints[i][j];
      }
   }

   first = 0;
   alldistance = 0;
   here = 0;
   /* initialization and centroids allocation */
   srand((unsigned int)time(NULL));
   for (i = 0; i < k; i++)
   {
      centroids[i].size = 0;
      centroids[i].center = (double *)calloc(d, sizeof(double));
      centroids[i].currentCenter = (double *)calloc(d, sizeof(double));
      /* random */

      random = (rand() % n);
      if (first == 0)
      {
         for (j = 0; j < d; j++)
         {
            centroids[i].center[j] = points[random].point[j];
            centroids[i].currentCenter[j] = 0;
         }
         first++;
      }
      else
      {
         for (p = 0; p < n; p++)
         {
            points[p].distance = findClosestCenter(points[p].point, centroids, d, i);
            alldistance += points[p].distance;
         }
         mergeSort(points, 0, n);
         p = n - 1;
         here = 0;

         while (here == 0)
         {
            if (p == 0)
            {
               p = n - 1;
            }
            randomNumber = (double)rand() / RAND_MAX;
            if ((points[p].distance / alldistance) > randomNumber)
            {
               /*To check if the chosen one is one of the centrals*/
               here = 1;
               for (j = 0; j < d; j++)
               {
                  centroids[i].center[j] = points[p].point[j];
                  centroids[i].currentCenter[j] = 0;
               }
            }
            p--;
         }
      }
   }
   printf("\n");
   printf("the smarter initial centers");
   printf("\n");
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
   printf("done");
   printf("\n");
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
      free(points[i].point);
   }
   free(points);

   for (i = 0; i < n; i++)
   {
      free(dataPoints[i]);
   }
   free(dataPoints);
   return returnValue;
}