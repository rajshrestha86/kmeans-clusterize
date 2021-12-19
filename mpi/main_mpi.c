//
// Created by pi on 12/5/21.
//

#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include"../lib/csvReader.h"
#include "kmeans_mpi.h"
#include "../lib/kmeans.h"
#include <mpi.h>

int main(int argc, char *argv[]){
    int comm_sz, rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int clusters = 6;
    int total_rows, columns, data_per_process;
    float *loc_data;
    float *loc_centroids, *global_centroids;
    int counts[comm_sz], displacements[comm_sz];
    double start_total, finish_total;
    start_total = MPI_Wtime();
    if(rank == 0){
        /*
         * Rank 0 is responsible in loading the data and distribute the data to other processes.
         */
        struct CSVFile csvData = readCsv("../data/genres_v2.csv");
        /*
         * Scatter the data among the processes.
         * Use displacements and data counts and use scatterv to
         * distribute data among the process.
         */
        data_per_process = csvData.rows / comm_sz;

        for (int i = 0; i < comm_sz; i++) {
            counts[i] = data_per_process * csvData.columns;
            displacements[i] = i * data_per_process * csvData.columns;
        }
        // Size of data may not be divisible by comm_sz, so send remaining to last process.
        counts[comm_sz - 1] = (csvData.rows - (comm_sz - 1) * data_per_process) * csvData.columns;
        // Broadcast some metadata so that other processes are ready to accept data.
        // 1. Send total number of rows to all process.
        // 2. Send number of columns in data.
        // 3. Send number
        total_rows = csvData.rows;
        columns = csvData.columns;
        MPI_Bcast(&total_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&columns, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&data_per_process, 1, MPI_INT, 0, MPI_COMM_WORLD);
        loc_data = malloc(sizeof (float ) * data_per_process * columns);
        MPI_Scatterv(csvData.data, counts, displacements, MPI_FLOAT, loc_data, data_per_process * columns, MPI_FLOAT, 0, MPI_COMM_WORLD);
        free(csvData.data);
    }

    // Receive scattered data from process 0.
    if(rank != 0){
        // Receive data sent by root process and start clustering data.
        MPI_Bcast(&total_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&columns, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&data_per_process, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if(rank == comm_sz - 1)
            data_per_process = total_rows - (comm_sz - 1) * data_per_process;

        loc_data = malloc(sizeof (float ) * data_per_process * columns);
        MPI_Scatterv(NULL, NULL, NULL, MPI_FLOAT, loc_data, (int) data_per_process * columns, MPI_FLOAT, 0,
                     MPI_COMM_WORLD);
    }
    /*
     * After data has been received every process has its own version of KMeans model.
     * The processes communicate with each other and update the centroids respectively after each iteration.
     */
    struct KMeans model = {
            .no_clusters = clusters,
            .centroids = malloc(sizeof (float ) * columns * clusters),
            .columns =  columns,
            .data = loc_data,
            .rows = data_per_process,
            .data_clusters = malloc(sizeof (int ) * data_per_process)
    };
    init_model(&model);
    global_centroids = malloc(sizeof (float ) * columns * clusters);
    for(int itr = 0; itr < 300; itr++){
        fit_mpi(&model);
        loc_centroids = model.centroids;
        /*
         * Reduction operation to compute average centroids.
         */
        MPI_Reduce(loc_centroids, global_centroids, columns * clusters, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
        if(rank == 0){
            /*
             * Rank 0 computes the average centroid between the processes and broadcast new centroids to everybody.
             */
            for(int k = 0; k < clusters; k++){
                for(int c = 0; c < columns; c++){
                    global_centroids[k * columns + c] = global_centroids[k * columns + c]/ (float ) comm_sz;
                }
            }
            printf("\nIteration: %d", itr);
        }
        MPI_Bcast(global_centroids, clusters * columns, MPI_FLOAT, 0, MPI_COMM_WORLD);
        // Replace current centroids with new updated centroids.
        replace_centroids(&model, global_centroids);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Write output to file one process at a time.
    MPI_Barrier(MPI_COMM_WORLD);
    char filename[] = "mpi_clusters.csv";
    int _rank = 0;
    MPI_Barrier (MPI_COMM_WORLD);
    while (_rank < comm_sz) {
        if (rank == _rank) {
            writeToCSV(&model, filename);
        }
        _rank ++;
        MPI_Barrier (MPI_COMM_WORLD);
    }

    if(rank == 0){
        printCentroids(&model);
        finish_total = MPI_Wtime();
        printf("\nTotal time elapsed: %f", finish_total - start_total);
    }
    MPI_Finalize();
    return 0;

}