#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void shaker_sort(int keys[], int vals[], int n) {
    int start = 0;
    int end = n - 1;
    int swapped = 1;

    while (swapped) {
        swapped = 0;
        // слева направо, находим макс
        for (int i = start; i < end; i++) {
            if (keys[i] > keys[i + 1]) {
                int temp = keys[i];
                keys[i] = keys[i + 1];
                keys[i + 1] = temp;

                temp = vals[i];
                vals[i] = vals[i + 1];
                vals[i + 1] = temp;

                swapped = 1;
            }
        }
        if (!swapped) {
            break;
        }
        end--;

        swapped = 0;
        // справа налево, находим мин
        for (int i = end; i > start; i--) {
            if (keys[i] < keys[i - 1]) {
                int temp = keys[i];
                keys[i] = keys[i - 1];
                keys[i - 1] = temp;

                temp = vals[i];
                vals[i] = vals[i - 1];
                vals[i - 1] = temp;

                swapped = 1;
            }
        }
        // не проверяем swapped, тк это проверит while
        start++;
    }

}

// значения всегда от 0 до n-1 !!!

void generate_sorted(int *keys, int *values, int n) {
    for (int i = 0; i < n; i++) {
        keys[i] = i;
        values[i] = i;
    }
}

void generate_reversed(int *keys, int *values, int n) {
    for (int i = 0; i < n; i++) {
        keys[i] = n - 1 - i;
        values[i] = i;
    }
}

// пытаемся сделать ключи неповторяющимися
void generate_random(int *keys, int *values, int n) {
    srand(time(NULL));
    
    for (int i = 0; i < n; i++) {
        int new_key= rand() % 1000;
        
        int unique = 1;
        for (int j = 0; j < i; j++) {
            if (keys[j] == new_key) {
                unique = 0;
                break;
            }
        }
        
        if (!unique) {
            // снижаем счетчик, чтобы цикл не завершился слишком рано
            i--;
            continue;
        }
        
        keys[i] = new_key;
        values[i] = i;
        
    }
}

void print_table(int *keys, int *values, int n) {
    printf("Ключи:   ");
    for (int i = 0; i < n; i++) {
        printf("%4d ", keys[i]);
    }
    printf("\nЗначения:");
    for (int i = 0; i < n; i++) {
        printf("%4d ", values[i]);
    }
    printf("\n");
}

int binary_search(int *keys, int n, int key) {
    int left = 0;
    int right = n - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (keys[mid] == key) {
            return mid;
        }
        else if (keys[mid] < key) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }
    
    return -1; // ключ не найден
}


int main() {
    int *keys = NULL;
    int *values = NULL;
    int n = 0;
    int choice = 0;

    while (1) {
        printf("Выберите команду:\n");
        printf("1 - Сгенерировать упорядоченную таблицу\n");
        printf("2 - Сгенерировать таблицу в обратном порядке\n");
        printf("3 - Сгенерировать таблицу со случайными значениями\n");
        printf("0 - Выход из программы\n");
        printf("Ваш выбор: ");

        if ((scanf("%d", &choice) != 1) || choice < 0 || choice > 3) {
            // очистка
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Некорректный ввод\n");
            continue;
        }

        if (choice == 0) {
            printf("Программа завершена.\n");
            break;
        }
        
        printf("Введите количество элементов (не менее 12): ");
        scanf("%d", &n);

        if (n < 12) {
            printf("Количество элементов должно быть не менее 12!\n");
            continue;
        }

        keys = (int *)malloc(n * sizeof(int));
        if (keys == NULL) {
            printf("Ошибка выделения памяти!\n");
            return 1;
        }

        values = (int *)malloc(n * sizeof(int));
        if (values == NULL) {
            printf("Ошибка выделения памяти!\n");
            free(keys);
            return 1;
        }

        switch (choice) {
            case 1:
                generate_sorted(keys, values, n);
                printf("\nСгенерирована упорядоченная таблица:\n");
                break;
            case 2:
                generate_reversed(keys, values, n);
                printf("\nСгенерирована таблица в обратном порядке:\n");
                break;
            case 3:
                generate_random(keys, values, n);
                printf("\nСгенерирована таблица со случайными ключами:\n");
                break;
        }

        print_table(keys, values, n);

        shaker_sort(keys, values, n);

        printf("\nОтсортированная таблица:\n");
        print_table(keys, values, n);
        
        // режим поиска значений по ключам
        printf("\nРежим поиска значений по ключам\n");
        printf("(для выхода введите -1)\n");
        
        int search_key = 0;
        while (1) {
            printf("\nВведите ключ для поиска (-1 для выхода): ");
            scanf("%d", &search_key);
            
            if (search_key == -1) {
                break;
            }
            
            int index = binary_search(keys, n, search_key);
            if (index != -1) {
                printf("Значение для ключа %d: %d\n", search_key, values[index]);
            } else {
                printf("Ключ %d не найден в таблице!\n", search_key);
            }
        }

        free(keys);
        free(values);

        printf("\nВозврат к главному меню...\n");
    }

    return 0;
}