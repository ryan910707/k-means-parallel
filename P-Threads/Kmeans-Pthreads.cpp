#include "lab1_pthread.h"
#include <bits/stdc++.h>
#include <malloc.h>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

using namespace std;
void kmeans_sequential(int N, int K, int *data_points, int **data_point_cluster, int **centroids, int *num_iterations);

vector<int> vect;
int num_threads;
int N;
int K;

struct Point
{
    double x;    // x coordinate
    double y;    // y coordinate
    double z;    // z coordinate
    int cluster; // cluster to which the point belongs
};
Point *points;
Point *centr;

// helper function prototypes
void mean_recompute(int K, int N, Point points[], Point centr[]);
Point addtwo(Point a, Point b);
double euclid(Point a, Point b);
Point mean(Point arr[], int N);
void assignclusters(Point points[], Point centr[], int K, int N);
void putback(Point centr[], int K);

// driver function

pthread_mutex_t lck;
void *assignparallely(void *tid)
{

    // double distances[K];
    // computing distance of a point and assigning all the data points, a centroid/cluster value
    int *id = (int *)tid;
    int length_per_thread = N / num_threads;
    int start = (*id) * length_per_thread;
    int stop = start + length_per_thread;
    if (*id == num_threads - 1)
        stop = N - 1;
    for (int i = start; i < stop; i++)
    {
        int min = 999999;
        int ind = 0;
        for (int j = 0; j < K; j++)
        {
            float d = euclid(points[i], centr[j]);
            if (d < min)
            {
                min = d;
                ind = j;
            }
        }
        // int index = 0;
        // for(int i = 1; i < K; i++)
        // {
        //     if(distances[i] < distances[index])
        //         index = i;
        // }
        // assigning the minimum distance cluster, which is an index
        pthread_mutex_lock(&lck);
        points[i].cluster = ind;
        pthread_mutex_unlock(&lck);
    }
    return NULL;
}

void kmeans_pthread(int num_threads, int N, int K, int *data_points, int **data_point_cluster, float **centroids, int *num_iterations)
{
    points = (Point *)malloc(sizeof(Point) * N);
    centr = (Point *)malloc(sizeof(Point) * K);
    ::num_threads = num_threads;
    ::K = K;

    //---------------------------
    int j = 0;
    for (int i = 0; i < (3 * N); i += 3)
    {
        points[j].x = data_points[i];
        points[j].y = data_points[i + 1];
        points[j].z = data_points[i + 2];
        j++;
    }

    //---------------------------

    // random centroid initialization. centroids are random points
    // center is the array of centroids containing their locations and cluster number as their index in this array
    srand(10);
    Point centr[K];
    for (int i = 0; i < K; i++)
    {
        int random = rand() % N; // some random value
        centr[i] = points[random];
    }

    //---------------------------

    // array to keep a track of distances of a point from all centroids, to take the minimum out of them
    double distances[K];
    // computing distance of a point and assigning all
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < K; j++)
        {
            distances[j] = euclid(points[i], centr[j]);
        }
        int index = 0;
        for (int i = 1; i < K; i++)
        {
            if (distances[i] < distances[index])
                index = i;
        }
        points[i].cluster = index;
    }

    //---------------------------

    mean_recompute(K, N, points, centr);
    putback(centr, K);

    //---------------------------

    int iterations = 1;
    int count;
    do
    {
        mean_recompute(K, N, points, centr);
        putback(centr, K);
        // storing old values for convergence check
        int old[N];
        for (int i = 0; i < N; i++)
        {
            old[i] = points[i].cluster;
        }
        int i = 0;
        int t = num_threads;
        pthread_t thr_arr[t];
        int *tid = (int *)malloc(sizeof(int) * t);
        pthread_mutex_init(&lck, NULL);
        /* parallel impl */
        for (i = 0; i < t; i++)
        { // i on heap?
            tid[i] = i;
            pthread_create(&thr_arr[i], NULL, assignparallely, &tid[i]);
        }
        for (i = 0; i < t; i++)
            pthread_join(thr_arr[i], NULL);

        // assignclusters(points, centr, K, N);
        iterations++;
        count = 0;
        for (int i = 0; i < N; i++)
        {
            if (old[i] == points[i].cluster)
                count++;
        }
    } while (count != N);

    //---------------------------

    *data_point_cluster = (int *)calloc(4 * N, sizeof(int));
    *centroids = (float *)calloc(vect.size(), sizeof(float));

    int q = 0;
    for (int i = 0; i < 4 * N; i += 4)
    {
        data_point_cluster[0][i] = points[q].x;
        data_point_cluster[0][i + 1] = points[q].y;
        data_point_cluster[0][i + 2] = points[q].z;
        data_point_cluster[0][i + 3] = points[q].cluster;
        q++;
    }
    for (int i = 0; i < vect.size(); i++)
    {
        centroids[0][i] = vect[i];
    }

    *num_iterations = vect.size() / K - 1;
}

// funtion to recompute the new centroids for each cluster
// N is the N number of data points and K is the N number of clusters
void mean_recompute(int K, int N, Point points[], Point centr[])
{
    int count[K];
    Point sum[K];
    for (int i = 0; i < N; i++)
    {
        count[points[i].cluster]++;
        sum[points[i].cluster] = addtwo(points[i], sum[points[i].cluster]);
    }
    for (int i = 0; i < K; i++)
    {
        centr[i].x = sum[i].x / count[i];
        centr[i].y = sum[i].y / count[i];
        centr[i].z = sum[i].z / count[i];
    }
}

// assuming they are the same cluster
Point addtwo(Point a, Point b)
{
    Point ans;
    ans.x = a.x + b.x;
    ans.y = a.y + b.y;
    ans.z = a.z + b.z;
    ans.cluster = a.cluster;
    return ans;
}

// function to calculate euclidea distance between two points
double euclid(Point a, Point b)
{
    double x = a.x - b.x;
    double y = a.y - b.y;
    double z = a.z - b.z;
    double dist = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    return dist;
}

void putback(Point centr[], int K)
{
    for (int i = 0; i < K; i++)
    {
        vect.push_back(centr[i].x);
        vect.push_back(centr[i].y);
        vect.push_back(centr[i].z);
    }
}
