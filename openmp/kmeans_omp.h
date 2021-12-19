//
// Created by pi on 12/3/21.
//

#ifndef HPCFINAL_KEMANS_H
#define HPCFINAL_KEMANS_H
#include "malloc.h"
#include "../lib/kmeans.h"


void init_model(struct KMeans *cluster);
void fit_omp(struct KMeans *model, int num_threads);
void writeToCSV(struct KMeans *model, char *filename);
#endif //HPCFINAL_KEMANS_H
