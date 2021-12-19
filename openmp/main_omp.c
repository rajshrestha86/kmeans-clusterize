//
// Created by pi on 12/5/21.
//

#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include"../lib/csvReader.h"
#include "kmeans_omp.h"
#include "../lib/kmeans.h"
#include <time.h>
#include "omp.h"
#define MAXCHAR 1000

int main(int argc, char *argv[]){
    int clusters = 6;
    // Read number of threads from command line.
    int threads = strtol(argv[1], NULL, 10);
    struct CSVFile csvData = readCsv("../data/genres_v2.csv");
    struct KMeans model = {
            .no_clusters = clusters,
            .centroids = malloc(sizeof (float ) * csvData . columns * clusters),
            .columns = csvData . columns,
            .data = csvData.data,
            .rows = csvData.rows,
            .data_clusters = malloc(sizeof (int ) * csvData.rows)
    };
    // Initialize model with random centroids.
   double start = omp_get_wtime();

    init_model(&model);
    // fit function made for openmp implementation.
    fit_omp(&model, threads);

    printCentroids(&model);
    printClusterCount(&model);
    double time_taken = omp_get_wtime() - start;
    printf("\n\nThe program took %f seconds to execute.\n\n", time_taken);

    char *resFile = "cluster_results_openmp.csv";
    writeToCSV(&model, resFile);
    return 0;

}