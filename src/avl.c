#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_KEY_LEN 6 // такая длина указана в задании

typedef struct Node {
    char key[MAX_KEY_LEN + 1];
    double value;
    struct Node *left;
    struct Node *right;
    int height; // высота поддерева, начинающегося из этого узла (нужно для балансировки). можно
    // каждый раз её вычислять (потратим время) или хранить (тратим память). Я выбрала хранить
} Node;

int height(Node *node) {
    if (node == NULL) {
        return 0;
    } else {
        return node->height;
    }
}

void updateHeight(Node *node) {
    if (node) {
        int left_height = height(node->left);
        int right_height = height(node->right);
        if (left_height > right_height) {
            node->height = left_height + 1;
        } else {
            node->height = right_height + 1;
        }
    }
}

// вычисление баланс-фактора (нужна ли балансировка, и если нужна, то какая)
int hightDifference(Node *node) {
    if (node == NULL) {
        return 0;
    } else {
        return height(node->left) - height(node->right);
    }
}

// малое правое вращение
Node* rotateRight(Node *y) {
    Node *x = y->left;
    Node *z = x->right;

    x->right = y;
    y->left = z;

    updateHeight(y);
    updateHeight(x);

    return x;
}

// малое левое вращение
Node* rotateLeft(Node *x) {
    Node *y = x->right;
    Node *z = y->left;

    y->left = x;
    x->right = z;

    updateHeight(x);
    updateHeight(y);

    return y;
}

Node* createNode(const char *key, double value) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->key, key);
    newNode->value = value;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->height = 1;
    return newNode;
}

int isValidKey(const char *key) {
    int len = strlen(key);
    if (len == 0 || len > MAX_KEY_LEN) {
        return 0;
    }
    for (int i = 0; i < len; i++) {
        if (!isalpha(key[i])) {
            return 0;
        }
    }
    return 1;
}

// вставка узла
Node* insert(Node *root, const char *key, double value, int *inserted) {
    // флаг inserted нужен для обработки успешности операции, чтобы записать нужное сообщение в файл
    if (!root) {
        *inserted = 1;
        return createNode(key, value);
    }
    
    int cmp = strcmp(key, root->key);
    
    if (cmp < 0) {
        root->left = insert(root->left, key, value, inserted);
    } else if (cmp > 0) {
        root->right = insert(root->right, key, value, inserted);
    } else {
        // ключ уже существовал
        *inserted = 0;
        return root;
    }
    updateHeight(root);

    // балансировка

    int hight_dif = hightDifference(root);
    
    // левый-левый случай (дисбаланс из-за левого потомка левого потомка рассматриваемого узла)
    // второе условие показывает, где именно произошел дисбаланс (в левом или правом поддереве потомка)
    if (hight_dif > 1 && strcmp(key, root->left->key) < 0) {
        return rotateRight(root);
    }
    // правый-правый случай
    if (hight_dif < -1 && strcmp(key, root->right->key) > 0) {
        return rotateLeft(root);
    }
    // случай лево-право
    if (hight_dif > 1 && strcmp(key, root->left->key) > 0) {
        root->left = rotateLeft(root->left); // большое правое вращение = малое левое + малое правое
        return rotateRight(root);
    }
    // случай право-лево
    if (hight_dif < -1 && strcmp(key, root->right->key) < 0) {
        root->right = rotateRight(root->right); // большое левое вращение = малое правое + малое левое
        return rotateLeft(root);
    }
    
    return root; // новый корень
}

// поиск узла с минимальным ключом
Node* minValueNode(Node *node) {
    Node *current = node;
    while (current && current->left) {
        current = current->left;
    }
    return current;
}

// удаление узла
Node* deleteNode(Node *root, const char *key, int *deleted) {
    // флаг deleted нужен для обработки успешности операции и записи нужного сообщения в выходной файл
    if (!root) {
        *deleted = 0;
        return NULL;
    }
    
    int cmp = strcmp(key, root->key);
    
    if (cmp < 0) {
        root->left = deleteNode(root->left, key, deleted);
    } else if (cmp > 0) {
        root->right = deleteNode(root->right, key, deleted);
    } else {
        *deleted = 1;
        
        // нет детей или один ребенок
        if (!root->left) {
            Node *temp = root->right;
            free(root);
            return temp;
        } else if (!root->right) {
            Node *temp = root->left;
            free(root);
            return temp;
        }
        
        // два ребенка
        Node *temp = minValueNode(root->right);
        strcpy(root->key, temp->key);
        root->value = temp->value;
        root->right = deleteNode(root->right, temp->key, deleted);
    }
    if (!root) return NULL;
    
    updateHeight(root);
    
    // Балансировка, все аналогично вставке узла

    int hight_dif = hightDifference(root);
    
    // левый-левый случай
    if (hight_dif > 1 && hightDifference(root->left) >= 0) {
        return rotateRight(root);
    }
    
    // левый-правый
    if (hight_dif > 1 && hightDifference(root->left) < 0) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }
    
    // правый-правый
    if (hight_dif < -1 && hightDifference(root->right) <= 0) {
        return rotateLeft(root);
    }
    
    // правый-левый
    if (hight_dif < -1 && hightDifference(root->right) > 0) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }
    
    return root;
}

// поиск значения по ключу
int search(Node *root, const char *key, double *value) {
    if (!root) return 0;
    
    int cmp = strcmp(key, root->key);
    
    if (cmp < 0) {
        return search(root->left, key, value);
    } else if (cmp > 0) {
        return search(root->right, key, value);
    } else {
        *value = root->value;
        return 1;
    }
}

// печать дерева
void print_tree(Node *root, FILE *output, int depth) { // вызывается с deapth=0
    if (root == NULL) {
        return;
    }

    print_tree(root->right, output, depth + 1);

    for (int i = 0; i < depth; i++) {
        fprintf(output, "              ");
    }
    fprintf(output, "%s %.6lf\n", root->key, root->value);

    print_tree(root->left, output, depth + 1);
}

// освобождение памяти
void freeTree(Node *root) {
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}


int main() {
    const char *inputFile = "input.txt";
    const char *outputFile = "output.txt";

    printf("Обрабатываются команды из файла: %s. Заполняйте его вашими командами перед запуском программы.\n", inputFile);
    printf("Формат ввода команд: <номер операции> [<ключ>] [<значение>]. Номера доступных команд:\n");
    printf("1 - Добавление узла с ключом и значением\n");
    printf("2 - Удаление узла по ключу\n");
    printf("3 - Печать дерева\n");
    printf("4 - Поиск занчения по ключу\n");
    

    FILE *input_file = fopen(inputFile, "r");
    FILE *output_file = fopen(outputFile, "w");
    
    if (input_file == NULL) {
        printf("Ошибка открытия входного файла!\n");
        return 0;
    }
    if (output_file == NULL) {
        printf("Ошибка открытия файла для вывода!\n");
        fclose(input_file);
        return 0;
    }
    
    Node *root = NULL;
    char line[256];
    int lineNum = 0;
    
    while (fgets(line, sizeof(line), input_file)) {
        lineNum++;
        line[strcspn(line, "\n")] = 0;
        
        int op;
        char key[MAX_KEY_LEN + 1];
        double value;
        
        int parsed = sscanf(line, "%d %s %lf", &op, key, &value);
        
        fprintf(output_file, "%s\n", line);
        
        switch (op) {
            case 1: { // добавление
                if (parsed != 3) {
                    fprintf(output_file, "Ошибка: неверный формат команды добавления\n");
                    break;
                }
                
                if (!isValidKey(key)) {
                    fprintf(output_file, "Ошибка: неверный ключ '%s' (должен быть 1-6 латинских букв)\n", key);
                    break;
                }
                
                int inserted = 0;
                root = insert(root, key, value, &inserted);
                
                if (inserted) {
                    fprintf(output_file, "Добавлен узел: %s = %lf\n", key, value);
                } else {
                    fprintf(output_file, "Ошибка: ключ '%s' уже существует\n", key);
                }
                break;
            }
            
            case 2: { // удаление
                if (parsed < 2) {
                    fprintf(output_file, "Ошибка: неверный формат команды удаления\n");
                    break;
                }
                
                int deleted = 0;
                root = deleteNode(root, key, &deleted);
                
                if (deleted) {
                    fprintf(output_file, "Удален узел с ключом: %s\n", key);
                } else {
                    fprintf(output_file, "Ошибка: ключ '%s' не найден\n", key);
                }
                break;
            }
            
            case 3: { // печать
                if (parsed != 1) {
                    fprintf(output_file, "Ошибка: команда печати не должна содержать параметров\n");
                    break;
                }
                
                if (!root) {
                    fprintf(output_file, "Дерево пусто\n");
                } else {
                    print_tree(root, output_file, 0);
                }
                break;
            }
            
            case 4: { // поиск
                if (parsed < 2) {
                    fprintf(output_file, "Ошибка: неверный формат команды поиска\n");
                    break;
                }
                
                double foundValue;
                if (search(root, key, &foundValue)) {
                    fprintf(output_file, "Найден: %s = %.6lf\n", key, foundValue);
                } else {
                    fprintf(output_file, "Ошибка: ключ '%s' не найден\n", key);
                }
                break;
            }
            
            default:
                fprintf(output_file, "Ошибка: неизвестная операция %d\n", op);
                break;
        }
        
        fprintf(output_file, "\n");
    }
    
    freeTree(root);
    fclose(input_file);
    fclose(output_file);


    printf("Обработка завершена. Результаты в файле: %s\n", outputFile);
    printf("В выходном файле сначала записана введенная команда, затем результат ее выполнения.\n");
    
    return 0;
}