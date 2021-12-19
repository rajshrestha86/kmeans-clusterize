#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include"lib/csvReader.h"
#include "lib/kmeans.h"
#include <time.h>
#define MAXCHAR 1000

int main(){
    int clusters = 6;
    // Read dataset and assign the elements to struct.
    struct CSVFile csvData = readCsv("data/genres_v2.csv");
    struct KMeans model = {
            .no_clusters = clusters,
            .centroids = malloc(sizeof (float ) * csvData . columns * clusters),
            .columns = csvData . columns,
            .data = csvData.data,
            .rows = csvData.rows,
            .data_clusters = malloc(sizeof (int ) * csvData.rows)
    };
    // Initialize the model with random centroids.
    init_model(&model);
    // Computing time taken to run the function.
    time_t t;
    t = time(NULL);
    // Run the KMeans clustering algorithm on data points.
    fit(&model);
    // Display the results.
    printCentroids(&model);
    printClusterCount(&model);
    time_t end = time(NULL);
    double time_taken = difftime(end, t);
    printf("\n\nThe program took %f seconds to execute.\n\n", time_taken);

    char *resFile = "cluster_results_serial.csv";
    writeToCSV(&model, resFile);
    return 0;

}