//
// Created by pi on 12/3/21.
//

#include <stdio.h>
#include "kmeans.h"
#include <malloc.h>
#include <stdlib.h>
#include <math.h>

#define SEED 100

void randomCentroids(struct KMeans *model) {
    /*
     * Generate a random number between 0 to number of data rows and
     * assigns the data point as a initial centroid to a model.
    */
    float *data = model->data;
    int rows = model->rows;
    int columns = model->columns;
    int clusters = model->no_clusters;
    for (int cluster = 0; cluster < clusters; cluster++) {
        // Between 0 & 1
        float random_number = ((float) rand() / (float) RAND_MAX);
        int random_row = (int) (random_number * rows);
        for (int i = 0; i < columns; i++) {
            model->centroids[columns * cluster + i] = data[random_row * columns + i];
        }
    }
}

float eucledianDist(float *pt1, struct KMeans *model, int pt2_index) {
    float dist = 0;
    for (int i = 0; i < model->columns; i++) {
        int data_index = model->columns * pt2_index + i;
        dist += (pt1[i] - model->data[data_index]) * (pt1[i] - model->data[data_index]);
    }
    return sqrtf(dist);
}

float *getClusterCentroid(struct KMeans *model, int cluster) {
    float *centroids = malloc(sizeof(float) * model->columns);
    for (int i = 0; i < model->columns; i++) {
        centroids[i] = model->centroids[cluster * model->columns + i];
    }
    return centroids;
}

void init_model(struct KMeans *cluster) {
    randomCentroids(cluster);
}


void assignCluster(const float *cluster_dist, struct KMeans *model, int index) {
    if (cluster_dist == NULL) {
        printf("Cluster distance not initialized while assigning....");
        exit(0);
    }
    float min_dist = cluster_dist[0];
    model->data_clusters[index] = 0;
    for (int i = 1; i < model->no_clusters; i++) {
        if (min_dist >= cluster_dist[i]) {
            min_dist = cluster_dist[i];
            model->data_clusters[index] = i;
        }
    }

}

void add_to_cluster_sum(int row, float *cluster_sum, int cluster, struct KMeans *model) {
    for (int i = 0; i < model->columns; i++) {
        cluster_sum[cluster * model->columns + i] += model->data[row * model->columns + i];
    }
}

void update_centroids(struct KMeans *model) {
    int cluster_count[model->no_clusters];
    float sum_cluster_columns[model->no_clusters * model->columns];
    // Initialize cluster count to 0.
    for (int i = 0; i < model->no_clusters; i++) {
        cluster_count[i] = 0;
        for (int c = 0; c < model->columns; c++) {
            sum_cluster_columns[i * model->columns + c] = 0;
        }
    }



    /*
     * Iterate through all data points and calculate sum of data points of columns and get count
     * for the given cluster. This will be used later to compute mean and update the centroid.
     */
    for (int row = 0; row < model->rows; row++) {
        for (int k = 0; k < model->no_clusters; k++) {
            if (model->data_clusters[row] == k) {
//                printf("\nCluster matched....");
                cluster_count[k] = cluster_count[k] + 1;
                add_to_cluster_sum(row, sum_cluster_columns, k, model);
                break;
            }
        }
    }

    // Update centroids with average
    for (int k = 0; k < model->no_clusters; k++) {
        for (int c = 0; c < model->columns; c++) {
            if (cluster_count[k] != 0)
                model->centroids[k * model->columns + c] =
                        sum_cluster_columns[k * model->columns + c] / (float) cluster_count[k];
        }

    }

//    printf("\nCentroids updated....");



}

void printCentroids(struct KMeans *model) {

    printf("\n printing centroids: \n");
    for (int cluster = 0; cluster < model->no_clusters; cluster++) {
        for (int i = 0; i < model->columns; i++) {
            float point = model->centroids[model->columns * cluster + i];
            printf("%f, ", point);
        }
        printf("\n");
    }
}

void printClusterCount(struct KMeans *model) {

    int cluster_count[model->no_clusters];
    for (int i = 0; i < model->no_clusters; i++)
        cluster_count[i] = 0;

    for (int row = 0; row < model->rows; row++) {
        for (int k = 0; k < model->no_clusters; k++) {
            if (model->data_clusters[row] == k) {
//                printf("\nCluster matched....");
                cluster_count[k] = cluster_count[k] + 1;
                break;
            }
        }
    }

    for (int i = 0; i < model->no_clusters; i++)
        printf("\nCount for cluster: %d = %d", i, cluster_count[i]);


}

void fit(struct KMeans *model) {
    /*
     * This is the serial implementation of KMeans clustering.
     * Start with random centroid, classify data points to cluster
     * re-compute centroid and repeat for 300 iterations.
     */
    printCentroids(model);
    float **cluster_centroids = malloc(sizeof(float *) * model->no_clusters);
    int iteration = 300;
    while (iteration >= 0) {

        for (int k = 0; k < model->no_clusters; k++) {
            cluster_centroids[k] = getClusterCentroid(model, k);
        }

        for (int i = 0; i < model->rows; i++) {
            float min_dist = eucledianDist(cluster_centroids[0], model, i);;
            model->data_clusters[i] = 0;
            for (int k = 1; k < model->no_clusters; k++) {
                float dist = eucledianDist(cluster_centroids[k], model, i);
                if (dist <= min_dist) {
                    min_dist = dist;
                    model->data_clusters[i] = k;
                }
            }
        }
        // After each iteration update the centroids.
        update_centroids(model);
        iteration--;
    }

}


void writeToCSV(struct KMeans *model, char *filename) {
    int row = model->rows;
    int columns = model->columns;
    float *data = model->data;
    FILE *fp;
    int row_index = 0;
    fp = fopen(filename, "a");
//    fp = fopen("/home/pi/CLionProjects/HPCFinal/data/genres_v2_read.csv", "w");
    while (row_index < row) {
        int column = 0;
        while (column < columns) {
            fprintf(fp, "%f,", data[row_index * columns + column]);
            column++;
        }
        fprintf(fp, "%d", model->data_clusters[row_index]);
        fprintf(fp, "\n");
        row_index++;
    }
    fclose(fp);
}