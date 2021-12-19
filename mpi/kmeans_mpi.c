//
// Created by pi on 12/3/21.
//

#include <stdio.h>
#include "kmeans_mpi.h"
#include <stdlib.h>
#include <mpi.h>
#define SEED 100


void replace_centroids(struct KMeans *model, float *new_centroids){
    for(int i = 0; i < model -> no_clusters; i++){
        for(int c = 0; c < model -> columns; c++){
            model -> centroids[i * model -> columns + c] = new_centroids[i * model -> columns + c];
        }
    }
}



void fit_mpi(struct KMeans *model) {
    /*
     * Similar to serial implementation but the iteration is only 1.
     */
    float **cluster_centroids = malloc(sizeof (float *) * model -> no_clusters);
    for (int k = 0; k < model->no_clusters; k++) {
        cluster_centroids[k] = getClusterCentroid(model, k);
    }
    for (int i = 0; i < model->rows; i++) {
        float min_dist =  eucledianDist(cluster_centroids[0], model, i);;
        model -> data_clusters[i] = 0;
        for (int k = 1; k < model->no_clusters; k++) {
            float dist = eucledianDist(cluster_centroids[k], model, i);
            if(dist <= min_dist){
                min_dist = dist;
                model -> data_clusters[i] = k;
            }
        }
    }
    update_centroids(model);
}


