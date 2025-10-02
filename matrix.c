#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <math.h>

#define MAX_LINHA 256

typedef struct data{
    float x;
    float y;
    float z;
    float w;
} Data;

typedef struct distance {
    Data v1;
    Data v2;
    float distance;
} Distance;

void readDataset(Data** datasetOut, int* nOut, char* fileName) {
    FILE* fp = fopen(fileName, "r");
    if(fp == NULL) {
        perror("\nError opening the file");
        exit(1);
    }

    char linha[MAX_LINHA];

    Data* dataset = NULL;
    int n = 0;

    while(fgets(linha, MAX_LINHA, fp)) {
        dataset = realloc(dataset, sizeof(Data)*(n+1));

        char variety[32];
        sscanf(linha, "%f,%f,%f,%f", 
               &dataset[n].x, &dataset[n].y,
               &dataset[n].z, &dataset[n].w);

        n++;
    }

    fclose(fp);

    *datasetOut = dataset;
    *nOut = n;
}

float euclidianDistance(Data a, Data b) {
    return sqrtf(
        (a.x - b.x) * (a.x - b.x) +
        (a.y - b.y) * (a.y - b.y) +
        (a.z - b.z) * (a.z - b.z) +
        (a.w - b.w) * (a.w - b.w)
    );
}

void euclidianDataset(Data* dataset, int n, Distance** distances, int* m) {
    *m = (n * (n-1)) / 2;
    *distances = malloc(sizeof(Distance)*(*m));

    int k = 0;
    for(int i = 0; i < n; i++) {
        for(int j = i + 1; j < n; j++) {
            (*distances)[k].v1 = dataset[i];
            (*distances)[k].v2 = dataset[j];
            (*distances)[k].distance = euclidianDistance(dataset[i], dataset[j]);
            k++;
        }
    }
}

void normalizeDistances(Distance* distances, int m) {
    float dmin = distances[0].distance;
    float dmax = distances[0].distance;

    for(int i = 0; i < m; i++) {
        if(distances[i].distance < dmin) dmin = distances[i].distance;
        if(distances[i].distance > dmax) dmax = distances[i].distance;
    }

    float range = dmax - dmin;
    if (range == 0) {
        for(int i = 0; i < m; i++) {
            distances[i].distance = 0.0f;
        }
        return;
    }

    for(int i = 0; i < m; i++) {
        distances[i].distance = (distances[i].distance - dmin) / (dmax - dmin);
    }
}

int** adjacencyMatrix(Distance* distances, int n, int m, float threshold) {
    int** matrix = malloc(sizeof(int*)*n);

    for(int i = 0; i < n; i++) {
        matrix[i] = malloc(sizeof(int)*n);
        for(int j = 0; j < n; j++) {
            matrix[i][j] = 0;
        }
    }

    int k = 0;
    for(int i = 0; i < n; i++) {
        for(int j = i + 1; j < n; j++) {
            if(distances[k].distance <= threshold) {
                matrix[i][j] = 1;
                matrix[j][i] = 1;
            }
            k++;
        }
    }

    return matrix;
}

void saveMatrixCSV(int** matrix, int n, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Error opening file");
        exit(1);
    }

    for(int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fprintf(fp, "%d", matrix[i][j]);
            if (j < n - 1) {
                fprintf(fp, ",");
            } 
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

int dfs(int v, int** matrix, int n, int* visited) {
    visited[v] = 1;
    int count = 1;
    printf("%d ", v);

    for(int i = 0; i < n; i++) {
        if(matrix[v][i] == 1 && !visited[i]) {
            count += dfs(i, matrix, n, visited);
        }
    }
    return count;
}

void connectedComponent(int** matrix, int n, char* filename) {
    int visited[n];
    for(int i = 0; i < n; i++) {
        visited[i] = 0;
    }

    FILE* fp = fopen(filename, "w");
    if(!fp) {
        perror("Error opening file");
        exit(1);
    }

    fprintf(fp, "component_id,size\n");
    int qtd = 0;

    for(int i = 0; i < n; i++) {
        if(!visited[i]) {
            qtd++;
            printf("Component %d: ", qtd);
            int size = dfs(i, matrix, n, visited);
            fprintf(fp, "%d,%d\n", qtd, size);
            printf("\n");
        }
    }

    fclose(fp);
    printf("Total components found: %d\n", qtd);
}

int main() {
    Data* dataset;
    int n;
    readDataset(&dataset, &n, "my_dataset.csv");

    Distance* distances;
    int m;
    euclidianDataset(dataset, n, &distances, &m);
    normalizeDistances(distances, m);

    int** matrix1 = adjacencyMatrix(distances, n, m, 0.0f);
    saveMatrixCSV(matrix1, n, "matrizes/matrix1.csv");

    int** matrix2 = adjacencyMatrix(distances, n, m, 0.3f);
    saveMatrixCSV(matrix2, n, "matrizes/matrix2.csv");

    int** matrix3 = adjacencyMatrix(distances, n, m, 0.5f);
    saveMatrixCSV(matrix3, n, "matrizes/matrix3.csv");

    int** matrix4 = adjacencyMatrix(distances, n, m, 0.9f);
    saveMatrixCSV(matrix4, n, "matrizes/matrix4.csv");

    printf("-------------- THRESHOLD 0,0 --------------\n");
    connectedComponent(matrix1, n, "clusters/components1.csv");
    printf("-------------- THRESHOLD 0,3 --------------\n");
    connectedComponent(matrix2, n, "clusters/components2.csv");
    printf("-------------- THRESHOLD 0,5 --------------\n");
    connectedComponent(matrix3, n, "clusters/components3.csv");
    printf("-------------- THRESHOLD 0,9 --------------\n");
    connectedComponent(matrix4, n, "clusters/components4.csv");
    
    for (int i = 0; i < n; i++) {
        free(matrix1[i]);
        free(matrix2[i]);
        free(matrix3[i]);
        free(matrix4[i]);
    }

    free(matrix1);
    free(matrix2);
    free(matrix3);
    free(matrix4);

    free(dataset);
    free(distances);

    return 0;
}
