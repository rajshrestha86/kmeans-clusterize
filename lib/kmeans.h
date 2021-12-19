//
// Created by pi on 12/3/21.
//

#ifndef HPCFINAL_KMEANS_H
#define HPCFINAL_KMEANS_H
#include "malloc.h"

struct KMeans{
    int no_clusters;
    float *centroids;
    int rows;
    int columns;
    float *data;
    int *data_clusters;
};

void randomCentroids(struct KMeans *model);
float eucledianDist(float *pt1, struct KMeans *model, int pt2_index);
float* getClusterCentroid(struct KMeans *model, int cluster);
void printCentroids(struct KMeans *model);
void printClusterCount(struct KMeans *model);
void init_model(struct KMeans *cluster);
void fit(struct KMeans *model);
void writeToCSV(struct KMeans *model, char *filename);
void update_centroids(struct KMeans *model);
#endif //HPCFINAL_KMEANS_H
