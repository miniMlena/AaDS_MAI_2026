#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_SIZE 100

// нахождение вершины с минимальным ключом
int min_key(int key[MAX_SIZE], int mst_set[MAX_SIZE], int vertices) {
    int min = INT_MAX;
    int min_index = -1;
    
    for (int v = 0; v < vertices; v++) {
        if (mst_set[v] == 0 && key[v] < min) {
            min = key[v];
            min_index = v;
        }
    }
    
    return min_index;
}

// построение минимального остовного дерева алгоритмом Прима
void prim_min_tree(int graph[MAX_SIZE][MAX_SIZE], int mst[MAX_SIZE][MAX_SIZE], int vertices) {
    int parent[MAX_SIZE];  // хранение родительских вершин
    int key[MAX_SIZE];     // хранение минимальных весов
    int mst_set[MAX_SIZE]; // отслеживание включенных вершин
    
    for (int i = 0; i < vertices; i++) {
        key[i] = INT_MAX;
        mst_set[i] = 0;
        for (int j = 0; j < vertices; j++) {
            mst[i][j] = 0;
        }
    }
    // в качестве начальной вершины я выбрала первую
    key[0] = 0;
    parent[0] = -1;
    
    for (int count = 0; count < vertices - 1; count++) {
        // выбираем вершину с минимальным ключом
        int u = min_key(key, mst_set, vertices);
        
        if (u == -1) break;
        
        mst_set[u] = 1;
        
        // обновляем ключи и родителей соседних вершин
        for (int v = 0; v < vertices; v++) {
            if (graph[u][v] != 0 && mst_set[v] == 0 && graph[u][v] < key[v]) {
                parent[v] = u;
                key[v] = graph[u][v];
            }
        }
    }
    
    // строим матрицу смежности для остовного дерева
    for (int i = 1; i < vertices; i++) {
        if (parent[i] != -1) {
            int weight = graph[parent[i]][i];
            mst[parent[i]][i] = weight;
            mst[i][parent[i]] = weight;
        }
    }
}

// вывод матрицы смежности
void print_matrix(int n, int matrix[MAX_SIZE][MAX_SIZE]) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int graph[MAX_SIZE][MAX_SIZE];
    int mst[MAX_SIZE][MAX_SIZE];
    int vertices;
    printf("Введите количество вершин: ");
    scanf("%d", &vertices);
    printf("Введите матрицу связности (%dx%d) (если вершины не связаны ставьте 0, иначе - вес ребра между ними):\n", vertices, vertices);
    for (int i = 0; i < vertices; i++) 
        for (int j = 0; j < vertices; j++)
            scanf("%d", &graph[i][j]);
    printf("Матрица смежности успешно считана\n");
    
    prim_min_tree(graph, mst, vertices);
    printf("Матрица смежности для получившегося минимального остовного дерева:\n");
    print_matrix(vertices, mst);
    
    return 0;
}