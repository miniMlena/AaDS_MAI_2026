#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *LeftChild;
    struct Node *RightChild;
} Node;

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
                printf("Вершина с таким значением уже существует.\n");
                free(new_node_ptr);
                return;
            }
        }
    }
}


void print_tree(Node *root, int depth) { // вызывается с deapth=0
    if (root == NULL) {
        return;
    }

    print_tree(root->RightChild, depth + 1);

    for (int i = 0; i < depth; i++)
        printf("  ");
    printf("%d\n", root->data);

    print_tree(root->LeftChild, depth + 1);
}

// возвращает указатель на указатель на узел! т.о. при удалении можно сразу вызвать удаление
// мин правой вершины, а не искать заново во всем правом поддереве. Если бы возвращался
// указатель на вершину, то это была бы копия, не связанная с самим деревом и был бы
// подвешенный указатель у родителя мин правой вершины
Node **find_min(Node **root_ptr) { // передается уже правая вершина, у которой мб нет левого ребенка
    while ((*root_ptr)->LeftChild != NULL) {
        root_ptr = &(*root_ptr)->LeftChild;
    }
    return root_ptr;
}

void delete(Node **root_ptr, int value) {
    if (root_ptr == NULL || *root_ptr == NULL) { //первое условие есть, тк нельзя разыменовывать нулевой указатель (segfault)
        printf("Нет вершины с таким значением.");
        return;
    }

    if (value < (*root_ptr)->data) {
        delete(&(*root_ptr)->LeftChild, value);
    } else if (value > (*root_ptr)->data) {
        delete(&(*root_ptr)->RightChild, value);
    } else {
        if ((*root_ptr)->LeftChild == NULL) {
            Node *tmp = *root_ptr;
            *root_ptr = (*root_ptr)->RightChild;
            free(tmp);
        } else if ((*root_ptr)->RightChild == NULL) {
            Node *tmp = *root_ptr;
            *root_ptr = (*root_ptr)->LeftChild;
            free(tmp);
        } else {
            Node **min_right_ptr = find_min(&(*root_ptr)->RightChild);
            (*root_ptr)->data = (*min_right_ptr)->data;
            // по сути ту же, что delete(min_right_ptr, value):
            Node *tmp = *min_right_ptr;
            *min_right_ptr = (*min_right_ptr)->RightChild;
            free(tmp);
        }
    }
}

int leaves_on_the_same_level(Node *root) {
    return 0;
}

int main() {
    Node *root = NULL; // дерево определяется указателем на его вершину
    add(&root, 10);
    add(&root, 6);
    add(&root, 15);
    add(&root, 9);
    add(&root, 8);
    add(&root, 7);
    add(&root, 5);
    add(&root, 4);
    add(&root, 16);
    add(&root, 12);
    print_tree(root, 0);

    return 0;
}