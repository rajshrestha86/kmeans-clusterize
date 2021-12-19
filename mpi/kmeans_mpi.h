//
// Created by pi on 12/3/21.
//

#ifndef HPCFINAL_KEMANS_H
#define HPCFINAL_KEMANS_H
#include "malloc.h"
#include "../lib/kmeans.h"


void fit_mpi(struct KMeans *model);
void replace_centroids(struct KMeans *model, float *new_centroids);
#endif //HPCFINAL_KEMANS_H
