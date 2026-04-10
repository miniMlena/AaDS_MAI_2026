#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#define MAX_EXPR 1000 // макс длина ввода

// типы узлов дерева
typedef enum {
    NODE_NUMBER,
    NODE_VARIABLE,
    NODE_OPERATOR
} NodeType;

// узел дерева выражения
typedef struct Node {
    NodeType type;
    union {
        int number;    // храним значение независимо от типа
        char variable;
        char op;
    } data;
    struct Node* left;
    struct Node* right;
} Node;

// стек для построения дерева
typedef struct {
    Node** items; // разница в типах содержимого
    int top;
    int size;
} Stack;

// стек для операторов
typedef struct {
    char* items;
    int top;
    int size;
} OpStack;

// Приоритет операторов
int get_priority(char op) {
    switch (op) { // используем проваливание без break
        case '+':
        case '-': return 1;
        case '*':
        case '/': return 2;
        case '^': return 3;
        default: return 0;
    }
}

int is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

// функции для стека операций
Stack* create_stack(int size) {
    Stack* s = malloc(sizeof(Stack));
    s->items = malloc(sizeof(Node*) * size);
    s->top = -1;
    s->size = size;
    return s;
}

void push_stack(Stack* s, Node* node) {
    if (s->top < s->size - 1) {
        s->items[++s->top] = node;
    }
}

Node* pop_stack(Stack* s) {
    if (s->top >= 0) {
        return s->items[s->top--];
    }
    return NULL;
}

void free_stack(Stack* s) {
    free(s->items);
    free(s);
}

// операции для стека операторов
OpStack* create_op_stack(int size) {
    OpStack* s = malloc(sizeof(OpStack));
    s->items = malloc(sizeof(char) * size);
    s->top = -1;
    s->size = size;
    return s;
}

void push_op(OpStack* s, char op) {
    if (s->top < s->size - 1) {
        s->items[++s->top] = op;
    }
}

char pop_op(OpStack* s) {
    if (s->top >= 0) {
        return s->items[s->top--];
    }
    return 0;
}

char peek_op(OpStack* s) {
    if (s->top >= 0) {
        return s->items[s->top];
    }
    return 0;
}

int is_empty_op(OpStack* s) {
    return s->top == -1;
}

void free_op_stack(OpStack* s) {
    free(s->items);
    free(s);
}

// Создание узлов
Node* create_number_node(int value) {
    Node* node = malloc(sizeof(Node));
    node->type = NODE_NUMBER;
    node->data.number = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

Node* create_variable_node(char var) {
    Node* node = malloc(sizeof(Node));
    node->type = NODE_VARIABLE;
    node->data.variable = var;
    node->left = NULL;
    node->right = NULL;
    return node;
}

Node* create_operator_node(char op, Node* left, Node* right) {
    Node* node = malloc(sizeof(Node));
    node->type = NODE_OPERATOR;
    node->data.op = op;
    node->left = left;
    node->right = right;
    return node;
}

// освобождение памяти
void free_tree(Node* root) {
    if (root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

void print_tree(Node *root, int depth) { // вызывается с deapth=0
    if (root == NULL) {
        return;
    }

    print_tree(root->right, depth + 1);

    for (int i = 0; i < depth; i++)
        printf("  ");
    switch (root->type) {
        case NODE_NUMBER:
            printf("%d\n", root->data.number);
            break;
        case NODE_VARIABLE:
            printf("%c\n", root->data.variable);
            break;
        case NODE_OPERATOR:
            printf("%c\n", root->data.op);
            break;
    }

    print_tree(root->left, depth + 1);
}

int is_left_associative(char op) {
    return op != '^';
}

int is_unary_minus(Node* node) {
    return node->type == NODE_OPERATOR && 
           node->data.op == '-' && 
           node->left == NULL;
}

// проверка на скобки (вспомогательная функция для корректной печати выражения)
int need_brackets(Node* parent, Node* child, int is_right_child) {
    if (child == NULL) return 0;
    if (child->type != NODE_OPERATOR) return 0;

    if (is_unary_minus(child)) return 1;
    
    int parent_prec = get_priority(parent->data.op);
    int child_prec = get_priority(child->data.op);

    if (parent->data.op == '^') {
        if (is_right_child) {
            return (child->type == NODE_NUMBER && child->data.number < 0) || child->type == NODE_OPERATOR;
        } else {
            return child_prec < parent_prec;
        }
    }
    if (child_prec < parent_prec) {
        return 1;
    }
    if (child_prec == parent_prec) {
        if (is_left_associative(parent->data.op)) {
            return is_right_child;
        } else {
            return !is_right_child;
        }
    }
    return 0;
}

// печать выражения (инфиксно)
void print_expression(Node* node) {
    if (node == NULL) return;

    if (node->type == NODE_NUMBER) {
        printf("%d", node->data.number);
        return;
    }
    if (node->type == NODE_VARIABLE) {
        printf("%c", node->data.variable);
        return;
    }
    // унарный минус
    if (is_unary_minus(node)) {
        printf("-");
        // если правый потомок - оператор с низким приоритетом, нужны скобки
        if (node->right->type == NODE_OPERATOR) {
            printf("(");
            print_expression(node->right);
            printf(")");
        } else {
            print_expression(node->right);
        }
        return;
    }
    int need_left_paren = (node->left != NULL && node->left->type == NODE_OPERATOR && need_brackets(node, node->left, 0));
    int need_right_paren = (node->right != NULL && node->right->type == NODE_OPERATOR && need_brackets(node, node->right, 1));

    if (need_left_paren) {
        printf("(");
        print_expression(node->left);
        printf(")");
    } else {
        print_expression(node->left);
    }

    printf(" %c ", node->data.op);

    if (need_right_paren) {
        printf("(");
        print_expression(node->right);
        printf(")");
    } else {
        print_expression(node->right);
    }
}



// построение дерева из выражения алгоритмом Дейкстры (стеком)
Node* build_tree(const char* expr) {
    OpStack* operators = create_op_stack(strlen(expr));
    Stack* output = create_stack(strlen(expr));
    
    int i = 0;
    int len = strlen(expr);
    
    while (i < len) {
        char c = expr[i];
        
        // пропускаем пробелы
        if (isspace(c)) {
            i++;
            continue;
        }
        // цифры собираем в числа
        if (isdigit(c)) {
            int num = 0;
            while (i < len && isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0'); // смещение, чтобы получить именно число, а не символ
                i++;
            }
            push_stack(output, create_number_node(num));
            continue;
        }
        // переменные
        if (isalpha(c)) {
            push_stack(output, create_variable_node(c));
            i++;
            continue;
        }
        // операторы
        if (is_operator(c)) {
            // обработка минуса с учетом унарного минуса
            if (c == '-' && (i == 0 || is_operator(expr[i-1]) || expr[i-1] == '(')) {
                i++;
                while (i < len && isspace(expr[i])) i++;

                if (isdigit(expr[i])) {
                    int num = 0;
                    while (i < len && isdigit(expr[i])) {
                        num = num * 10 + (expr[i] - '0');
                        i++;
                    }
                    push_stack(output, create_number_node(-num));
                } else if (isalpha(expr[i])) {
                    Node* var_node = create_variable_node(expr[i]);
                    // создаем такую структуру: в корне -, слева ничего, справа переменная ( -а)
                    push_stack(output, create_operator_node('-', NULL, var_node));
                    i++;
                } else if (expr[i] == '(') {
                    // рекурсивно парсим выражение в скобках
                    int bracket_count = 1; //одна уже есть
                    int start = i + 1;
                    int j = start;
                    while (j < len && bracket_count > 0) {
                        if (expr[j] == '(') bracket_count++;
                        if (expr[j] == ')') bracket_count--;
                        j++;
                    }
                    char* sub_expr = malloc(j - start);
                    strncpy(sub_expr, expr + start, j - start - 1);
                    sub_expr[j - start - 1] = '\0';
                    Node* sub_node = build_tree(sub_expr);
                    free(sub_expr);
                    push_stack(output, create_operator_node('-', NULL, sub_node));
                    i = j;
                }
                continue;
            }
            // Обрабатываем стек по алгоритму
            while (!is_empty_op(operators) && peek_op(operators) != '(') {
                char top_op = peek_op(operators);
                if ((get_priority(top_op) > get_priority(c)) ||
                    (get_priority(top_op) == get_priority(c) && is_left_associative(c))) {
                    char op = pop_op(operators);
                    Node* right = pop_stack(output);
                    Node* left = pop_stack(output);
                    push_stack(output, create_operator_node(op, left, right));
                } else {
                    break;
                }
            }
            push_op(operators, c);
            i++;
            continue;
        }

        if (c == '(') {
            push_op(operators, c);
            i++;
            continue;
        }

        if (c == ')') {
            while (!is_empty_op(operators) && peek_op(operators) != '(') {
                char op = pop_op(operators);
                Node* right = pop_stack(output);
                Node* left = pop_stack(output);
                push_stack(output, create_operator_node(op, left, right));
            }
            if (!is_empty_op(operators) && peek_op(operators) == '(') {
                pop_op(operators); // удаляем (
            }
            i++;
            continue;
        }
        i++;
    }
    // выталкиваем оставшиеся операторы
    while (!is_empty_op(operators)) {
        char op = pop_op(operators);
        Node* right = pop_stack(output);
        Node* left = pop_stack(output);
        push_stack(output, create_operator_node(op, left, right));
    }
    
    Node* result = pop_stack(output);
    
    free_op_stack(operators);
    free_stack(output);
    
    return result;
}

// копирование дерева
Node* copy_tree(Node* root) {
    if (root == NULL) return NULL;
    
    Node* new_node = malloc(sizeof(Node));
    memcpy(new_node, root, sizeof(Node));
    
    new_node->left = copy_tree(root->left);
    new_node->right = copy_tree(root->right);
    
    return new_node;
}

// смотрим, совпадают ли множители
int compare_trees(Node* a, Node* b) {
    if (a == NULL && b == NULL) return 1;
    if (a == NULL || b == NULL) return 0;
    if (a->type != b->type) return 0;
    
    switch (a->type) { // у второго такой же тип
        case NODE_NUMBER:
            return a->data.number == b->data.number;
        case NODE_VARIABLE:
            return a->data.variable == b->data.variable;
        case NODE_OPERATOR:
            if (a->data.op != b->data.op) return 0;
            // для коммутативных операторов проверяем оба порядка
            if (a->data.op == '+' || a->data.op == '*') {
                return (compare_trees(a->left, b->left) && compare_trees(a->right, b->right)) ||
                       (compare_trees(a->left, b->right) && compare_trees(a->right, b->left));
            } else {
                return compare_trees(a->left, b->left) && compare_trees(a->right, b->right);
            }
        default:
            return 0;
    }
}

// Преобразование разности с выносом общих множителей
Node* common_factors_from_difference(Node* root) {
    if (root == NULL) {
        return NULL;
    }

    root->left = common_factors_from_difference(root->left);
    root->right = common_factors_from_difference(root->right);
    
    // обрабатываем только узлы вычитания
    if (root->type == NODE_OPERATOR && root->data.op == '-') {
        Node* left_expr = root->left;
        Node* right_expr = root->right;

        Node* left_factor = NULL;
        Node* right_factor = NULL;
        Node* left_rest = NULL;
        Node* right_rest = NULL;
        
        // нужно, чтобы оба поддерева были произведениями или одиночными множители
        // левая часть - умножение
        if (left_expr && left_expr->type == NODE_OPERATOR && left_expr->data.op == '*') {
            // пправая часть - умножение
            if (right_expr && right_expr->type == NODE_OPERATOR && right_expr->data.op == '*') {
                // сравниваем левые и правые части умножения
                if (compare_trees(left_expr->left, right_expr->left)) {
                    // копирование нужно, т.к. root мы удалим вместе с его врешинами и
                    // без копии мы потеряем те значения, которые положили в новые узлы
                    left_factor = copy_tree(left_expr->left);
                    left_rest = copy_tree(left_expr->right);
                    right_rest = copy_tree(right_expr->right);
                } else if (compare_trees(left_expr->left, right_expr->right)) {
                    left_factor = copy_tree(left_expr->left);
                    left_rest = copy_tree(left_expr->right);
                    right_rest = copy_tree(right_expr->left);
                } else if (compare_trees(left_expr->right, right_expr->left)) {
                    left_factor = copy_tree(left_expr->right);
                    left_rest = copy_tree(left_expr->left);
                    right_rest = copy_tree(right_expr->right);
                } else if (compare_trees(left_expr->right, right_expr->right)) {
                    left_factor = copy_tree(left_expr->right);
                    left_rest = copy_tree(left_expr->left);
                    right_rest = copy_tree(right_expr->left);
                }
            } else {
                // правая часть - не умножение, но может быть общим множителем
                if (compare_trees(left_expr->left, right_expr)) {
                    left_factor = copy_tree(left_expr->left);
                    left_rest = copy_tree(left_expr->right);
                    right_rest = create_number_node(1);
                } else if (compare_trees(left_expr->right, right_expr)) {
                    left_factor = copy_tree(left_expr->right);
                    left_rest = copy_tree(left_expr->left);
                    right_rest = create_number_node(1);
                }
            }
        } else if (right_expr && right_expr->type == NODE_OPERATOR && right_expr->data.op == '*') {
            // левая часть - не умножение
            if (compare_trees(right_expr->left, left_expr)) {
                left_factor = copy_tree(right_expr->left);
                left_rest = create_number_node(1);
                right_rest = copy_tree(right_expr->right);
            } else if (compare_trees(right_expr->right, left_expr)) {
                left_factor = copy_tree(right_expr->right);
                left_rest = create_number_node(1);
                right_rest = copy_tree(right_expr->left);
            }
        }
        
        // если нашли общий множитель
        if (left_factor != NULL) {
            Node* diff = create_operator_node('-', left_rest, right_rest);
            Node* result = create_operator_node('*', left_factor, diff);
            free_tree(root);
            return result;
        }
    }
    
    return root;
}

int main() {
    char expr[MAX_EXPR];

    printf("Вынесение общих множителей из разности\n");
    printf("Поддерживаемые операции: +, -, *, /, ^, скобки\n");
    printf("Пример выражения: a * x - x * (c - d)\n");
    printf("Введите арифметическое выражение: ");
    
    fgets(expr, MAX_EXPR, stdin);
    expr[strcspn(expr, "\n")] = 0; // удаляем символ новой строки
    
    printf("\nИсходное выражение: %s\n", expr);

    Node* tree = build_tree(expr);
    
    if (tree == NULL) {
        printf("Ошибка при построении дерева!\n");
        return 1;
    }

    printf("\nДерево выражения:\n");
    print_tree(tree, 0);

    tree = common_factors_from_difference(tree);
    
    printf("\nПреобразованное выражение: ");
    print_expression(tree);
    printf("\n");
    
    printf("\nДерево преобразованного выражения:\n");
    print_tree(tree, 0);

    // очистка памяти
    free_tree(tree);

    return 0;
}