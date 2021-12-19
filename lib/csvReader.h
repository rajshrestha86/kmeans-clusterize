//
// Created by pi on 12/2/21.
//

#ifndef HPCFINAL_CSVREADER_H
#define HPCFINAL_CSVREADER_H

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAXCHAR 1000
#define COLUMNS 6


struct CSVFile{
    float *data;
    int rows;
    int columns;
};

int getLines(FILE *fp){
    int line = 0;
    int c = fgetc(fp);
    char t;
    while (c != EOF) {
        t = (char) c;
        if (t == '\n') {
            line+=1;
        }

        c = fgetc(fp);
    }
    printf("\nThis is line number: %d", (int) line);
    return (int ) line - 1;
}


/*
 * This function checks if the column that we are parsing from csv is required column or not.
 * If the column is valid we would use it for the clustering if not we would discard it.
 */
bool isColumnValid(int column_no){
    column_no++;
    switch (column_no) {
        case 1:
        case 2:
        case 4:
        case 6:
        case 9:
        case 10:
            return true;
        default:
            return false;
    }

}


void writeCSV(int row, int columns, float *data) {
    FILE *fp;
    int row_index = 0;
    fp = fopen("data/genres_v2_read.csv", "w");
    while (row_index < row) {
        int column = 0;
        while(column < columns){
            fprintf(fp, "%f", data[row_index * columns + column]);
            column++;
            if(column != columns)
                fprintf(fp, ",");
        }
        fprintf(fp, "\n");
        row_index++;
    }
    fclose(fp);
}



struct CSVFile readCsv(char *filename){
    FILE *fp;
    float *data;
    char row[MAXCHAR];
    char *token;
    fp = fopen(filename,"r");
    int total_rows = getLines(fp);
    rewind(fp);
    if(fp == NULL){
        printf("Null file read.... ");
        exit(0);
    }
    data = (float *) malloc(sizeof (float) * total_rows * COLUMNS);
    int row_no = -1;
    while (fgets(row, MAXCHAR, fp)) {
        if(row_no == -1){
            row_no++;
            continue;
        }

        int column = 0;
        int index = 0;
        token = strtok(row, ",");
        while (token != NULL) {
            if (isColumnValid(column)) {
                char *endp;
                float value = strtof(token, &endp);
                data[row_no * COLUMNS + index] = value;
//                printf("%d, %d: %d\n", row_no * COLUMNS + index, column, index, value);
                index++;
            }
            column += 1;
            token = strtok(NULL, ",");
        }
        row_no += 1;
    }
    struct CSVFile _file = {data, total_rows, COLUMNS};
    return _file;

}


#endif //HPCFINAL_CSVREADER_H
