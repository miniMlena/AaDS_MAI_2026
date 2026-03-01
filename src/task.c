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
        printf("MEMORY ERROR\n");
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
        printf("Нет вершины с таким значением: %d.\n", value);
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

int same_leaves_level(Node *root) {
    return 0;
}

int main() {
    Node *root = NULL; // дерево определяется указателем на его вершину
    int command = 0;
    int value = 0;

    printf("\nДоступные команды:\n");
    printf("1 - Добавить вершину\n");
    printf("2 - Удалить вершину\n");
    printf("3 - Напечатать дерево\n");
    printf("4 - Проверить, что все листья на одном уровне\n");
    printf("5 - Посмотреть это сообщение\n");
    printf("0 - Выход\n");

    do {
        printf("Введите команду: ");

        if (scanf("%d", &command) != 1) {
            // очистка
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Некорректный ввод.\n");
            continue;
        }

        switch (command) {
        case 1:
            printf("Введите целое число для добавления: ");
            if (scanf("%d", &value) == 1) {
                add(&root, value);
            } else {
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {}
                printf("Некорректное значение.\n");
            }
            break;
        case 2:
            printf("Введите ключ для удаления: ");
            if (scanf("%d", &value) == 1) {
                delete(&root, value);
            } else {
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {}
                printf("Некорректное значение.\n");
            }
            break;
        case 3:
            printf("Текущее дерево:\n");
            print_tree(root, 0);
            break;
        case 4:
            if (same_leaves_level(root))
                printf("Все листья находятся на одном уровне.\n");
            else
                printf("Листья находятся на разных уровнях.\n");
            break;
        case 5:
            printf("Доступные команды:\n");
            printf("1 - Добавить вершину\n");
            printf("2 - Удалить вершину\n");
            printf("3 - Напечатать дерево\n");
            printf("4 - Проверить, что все листья на одном уровне\n");
            printf("5 - Посмотреть это сообщение\n");
            printf("0 - Выход\n");
            break;
        case 0:
            printf("Выход.\n");
            break;
        default:
            printf("Неизвестная команда.\n");
            break;
        }

    } while (command != 0);

    return 0;
}