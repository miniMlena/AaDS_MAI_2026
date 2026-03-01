#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *LeftChild;
    struct Node *RightChild;
} Node;

// Node *root = NULL;

void add(Node** root_ptr, int value) { // ** чтобы можно было менять
    Node *new_node_ptr = (Node*)malloc(sizeof(Node));
    if (new_node_ptr == NULL) {
        printf("MEMORY ERROR");
        return;
    }
    new_node_ptr->data = value;
    new_node_ptr->LeftChild = NULL;
    new_node_ptr->RightChild = NULL;

    if (*root_ptr == NULL) {
        *root_ptr = new_node_ptr;
    } else {
        Node *parent = *root_ptr; // возможный родитель для нового узла
        while (1) {
            if (value < parent->data) {
                if (parent->LeftChild == NULL) {
                    parent->LeftChild = new_node_ptr;
                    return;
                } else {
                    parent = parent->LeftChild;
                }
            } else if (value > parent->data) {
                if (parent->RightChild == NULL) {
                    parent->RightChild = new_node_ptr;
                    return;
                } else {
                    parent = parent->RightChild;
                }
            } else {
                printf("Вершина с таким значением уже существует\n");
                free(new_node_ptr);
                return;
            }
        }
    }
}

void print_tree(Node *root) { //не нужно менять

}

void delete(Node **root_ptr, int value) {
    if (value )
}

int leaves_on_the_same_level(Node *root) {
    return 0;
}

int main() {
    Node *root = NULL; // дерево определяется указателем на его вершину

    return 0;
}