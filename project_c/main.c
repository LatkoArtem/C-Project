#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>

#define MAX_CITIES 20
#define MAX_CITY_NAME_LENGTH 19

int check_for_string(char* text, char* variable) {
    while (true) {
        // Введення рядка
        printf("%s", text);
        fgets(variable, MAX_CITY_NAME_LENGTH, stdin);
        // Перевірка, чи введено щось
        if (variable[0] == '\n') {
            printf("Incorrect input\n");
            continue;
        }
        // Видалення символу нового рядка
        variable[strcspn(variable, "\n")] = '\0';
        if (strlen(variable) >= MAX_CITY_NAME_LENGTH - 1) {
            printf("Too many characters. The number of characters must not exceed 17 characters\n");
            // Очистити введений рядок
            while (getchar() != '\n');
            continue;
        }
        break;
    }
    return 0;
}

int check_for_int(char* text, int* variable) {
    char input[50];
    while (true) {
        printf("%s", text);
        fgets(input, sizeof(input), stdin);
        // Перевірити, чи є крапка в рядку введення
        if (strchr(input, '.') != NULL) {
            printf("Incorrect input\n");
            continue;
        }
        if (sscanf(input, "%d", variable) != 1) {
            printf("Incorrect input\n");
            continue;
        }
        // Перевірити, чи введене значення має непотрібні символи
        char* endptr;
        strtol(input, &endptr, 10);
        if (*endptr != '\0' && *endptr != '\n') {
            printf("Incorrect input\n");
            continue;
        }
        break;
    }
    return 0;
}

int check_for_float(char* text, float* variable) {
    char input[50];
    while (true) {
        printf("%s", text);
        fgets(input, sizeof(input), stdin);
        if (sscanf(input, "%f", variable) != 1) {
            printf("Incorrect input\n");
            continue;
        }
        // Перевірити, чи введене значення має непотрібні символи
        char* endptr;
        strtof(input, &endptr);
        if (*endptr != '\0' && *endptr != '\n') {
            printf("Incorrect input\n");
            continue;
        }
        break;
    }
    return 0;
}

// Функція для обміну значень
void swap(int* x, int* y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

// Рекурсивна функція для генерації всіх перестановок
void permute(int cities_num, int graph[][cities_num], int path[], int l, int r, int* minCost, int minPath[]) {
    if (l == r) {
        int currentCost = 0;
        for (int i = 0; i < cities_num - 1; i++) {
            currentCost += graph[path[i]][path[i + 1]];
        }
        currentCost += graph[path[cities_num - 1]][path[0]];
        if (currentCost < *minCost) {
            *minCost = currentCost;
            for (int i = 0; i < cities_num; i++) {
                minPath[i] = path[i];
            }
        }
    }
    else {
        for (int i = l; i <= r; i++) {
            swap(&path[l], &path[i]);
            permute(cities_num, graph, path, l + 1, r, minCost, minPath);
            swap(&path[l], &path[i]); // Повертаємо до початкового стану
        }
    }
}

// Функція для обчислення найкоротшого шляху
void tspBruteForce(int cities_num, int graph[cities_num][cities_num], int start_city, char** city_names) {
    int path[cities_num];
    for (int i = 0; i < cities_num; i++) {
        path[i] = i;
    }

    // Переставляємо початкове місто на початок шляху
    int temp = path[0];
    path[0] = path[start_city];
    path[start_city] = temp;

    int minCost = INT_MAX;
    int minPath[cities_num];

    permute(cities_num, graph, path, 1, cities_num - 1, &minCost, minPath);

    printf("Shortest way: ");
    for (int i = 0; i < cities_num; i++) {
        printf("%s -> ", city_names[minPath[i]]);
    }
    printf("%s", city_names[start_city]);
    printf("\nDistance of way: %d\n", minCost);
    printf("\n");
}


// Структура для зберігання інформації про шлях
typedef struct {
    float time;
    float cost;
} Edge;

// Черга з пріоритетом для перестановок Хіпа
typedef struct {
    int city;
    float time;
    float cost;
} PriorityQueue;

PriorityQueue heap[MAX_CITIES * MAX_CITIES]; // Підтримуємо максимальний розмір черги
int heapSize;

// Функція для додавання елемента до черги
void push(PriorityQueue element) {
    heap[++heapSize] = element;
    int now = heapSize;
    while (now > 1 && heap[now / 2].time + heap[now / 2].cost > heap[now].time + heap[now].cost) {
        PriorityQueue temp = heap[now / 2];
        heap[now / 2] = heap[now];
        heap[now] = temp;
        now /= 2;
    }
}

// Функція для видалення найменшого елемента з черги
PriorityQueue pop() {
    PriorityQueue minElement = heap[1];
    heap[1] = heap[heapSize--];
    int now = 1, child = 2;
    while (child <= heapSize) {
        if (child < heapSize && heap[child].time + heap[child].cost > heap[child + 1].time + heap[child + 1].cost) {
            ++child;
        }
        if (heap[now].time + heap[now].cost > heap[child].time + heap[child].cost) {
            PriorityQueue temp = heap[now];
            heap[now] = heap[child];
            heap[child] = temp;
            now = child;
            child *= 2;
        } else {
            break;
        }
    }
    return minElement;
}

// Функція для знаходження оптимального маршруту
void optimalRoute(int cities_num, Edge graph[cities_num][cities_num], int start_city, int end_city, char** city_names) {
    float minTime[MAX_CITIES], minCost[MAX_CITIES];
    int prevCity[MAX_CITIES];
    bool visited[MAX_CITIES];
    for (int i = 0; i < cities_num; ++i) {
        minTime[i] = FLT_MAX;
        minCost[i] = FLT_MAX;
        prevCity[i] = -1; // Ініціалізуємо попереднє місто як -1
        visited[i] = false;
    }

    minTime[start_city] = 0.0;
    minCost[start_city] = 0.0;

    PriorityQueue start;
    start.city = start_city;
    start.time = 0;
    start.cost = 0;

    push(start);

    while (heapSize > 0) {
        PriorityQueue u = pop();
        if (visited[u.city]) {
            continue;
        }
        visited[u.city] = true;

        for (int v = 0; v < cities_num; ++v) {
            if (graph[u.city][v].time != FLT_MAX && graph[u.city][v].cost != FLT_MAX && !visited[v]) {
                float newTime = minTime[u.city] + graph[u.city][v].time;
                float newCost = minCost[u.city] + graph[u.city][v].cost;
                if (newTime < minTime[v] || (newTime == minTime[v] && newCost < minCost[v])) {
                    minTime[v] = newTime;
                    minCost[v] = newCost;
                    prevCity[v] = u.city; // Запам'ятовуємо попереднє місто для поточного міста
                    PriorityQueue next;
                    next.city = v;
                    next.time = newTime;
                    next.cost = newCost;
                    push(next);
                }
            }
        }
    }

    // Виведення інформації про кожен шлях
    printf("\n                        Optimal route information       \n");
    printf("_____________________________________________________________________________");
    printf("\n| %-17s| %-17s| %-17s| %-17s|\n", "From City", "To City", "Time(Hours)", "Cost(UAH)");
    printf("|___________________________________________________________________________|\n");
    int current_city = end_city;
    float totalTime = 0.0;
    float totalCost = 0.0;
    while (current_city != start_city) {
        int prev_city = prevCity[current_city];
        totalTime += graph[prev_city][current_city].time; // Оновлюємо загальний час, додаючи час поточного маршруту
        totalCost += graph[prev_city][current_city].cost; // Оновлюємо загальну вартість, додаючи вартість поточного маршруту
        printf("| %-17s| %-17s| %-17.2f| %-17.2f|\n", city_names[prev_city], city_names[current_city], graph[prev_city][current_city].time, graph[prev_city][current_city].cost);
        current_city = prev_city;
    }
    printf("|___________________________________________________________________________|\n");
    printf("|                                  TOTAL:                                   |\n");
    printf("|___________________________________________________________________________|\n");
    printf("| %-17s| %-17s| %-17.2f| %-17.2f|\n", city_names[start_city], city_names[end_city], totalTime, totalCost);
    printf("|___________________________________________________________________________|\n\n");
}

int find_city_index(char** city_names, int cities_num, char* city_name) {
    for (int i = 0; i < cities_num; i++) {
        if (strcmp(city_names[i], city_name) == 0) {
            return i;
        }
    }
    return -1; // Якщо місто не знайдено
}



int main() {
    printf("\n-------------Travelling Salesman Problem---------------\n\n");
    int choice;

    while (true) {
        check_for_int("1 - Program #1, 2 - Program #2, 0 - EXIT. Enter 1, 2 or 0: ", &choice);
        if (choice == 1) {
            int cities_num;
            while(true){
                check_for_int("Enter the number of cities (from 2 to 20): ", &cities_num);
                if (cities_num <= 1 || cities_num > MAX_CITIES) {
                    printf("Incorrect input. Please enter a number of cities from 2 to 20.\n");
                    continue;
                }
                break;
            }
            int graph[cities_num][cities_num];
            bool isVisited[cities_num][cities_num]; // Масив для відстеження введених відстаней
            // Ініціалізуємо масив isVisited значеннями false
            for (int i = 0; i < cities_num; i++) {
                for (int j = 0; j < cities_num; j++) {
                    isVisited[i][j] = false;
                }
            }

            char** city_names = (char**)malloc(cities_num * sizeof(char*));
            for (int i = 0; i < cities_num; i++) {
                city_names[i] = (char*)malloc(MAX_CITY_NAME_LENGTH * sizeof(char));
                while (true) {
                    check_for_string("Enter name of city: ", city_names[i]);
                    if (find_city_index(city_names, i, city_names[i]) != -1) {
                        printf("City name already exists. Please enter a different name.\n");
                        continue;
                    }
                    break;
                }
            }

            for (int i = 0; i < cities_num; i++) {
                for (int j = i + 1; j < cities_num; j++) { // Зчитуємо відстані тільки для верхньої частини матриці
                    if (!isVisited[j][i]) { // Перевіряємо, чи не була введена відстань для оберненої пари міст
                        while (true) {
                            printf("Distance from city %s to city %s. ", city_names[i], city_names[j]);
                            check_for_int("Enter distance: ", &graph[i][j]);
                            graph[j][i] = graph[i][j]; // Встановлюємо відстань для оберненої пари міст та відзначаємо, що вона була введена
                            isVisited[i][j] = isVisited[j][i] = 1;
                            break;
                        }
                    }
                }
                // Відстань від міста до себе завжди нуль
                graph[i][i] = 0;
            }
            // Вибираємо місто, з якого починати обхід
            char start_city_name[MAX_CITY_NAME_LENGTH];
            int start_city_index;
            while (true) {
                check_for_string("Enter the starting city: ", start_city_name);
                start_city_index = find_city_index(city_names, cities_num, start_city_name);
                if (start_city_index == -1) {
                    printf("City not found.\n");
                    continue;
                }
                break;
            }
            tspBruteForce(cities_num, graph, start_city_index, city_names);

            // Free allocated memory for city names
            for (int i = 0; i < cities_num; i++) {
                free(city_names[i]);
            }
            free(city_names);
        }
        else if (choice == 2) {
            int cities_num;
            while(true){
                check_for_int("Enter the number of cities (from 2 to 20): ", &cities_num);
                if (cities_num <= 1 || cities_num > MAX_CITIES) {
                    printf("Incorrect input. Please enter a number of cities from 2 to 20.\n");
                    continue;
                }
                break;
            }
            Edge graph[cities_num][cities_num];
            char** city_names = (char**)malloc(cities_num * sizeof(char*));
            for (int i = 0; i < cities_num; i++) {
                city_names[i] = (char*)malloc(MAX_CITY_NAME_LENGTH * sizeof(char));
                while(true){
                    check_for_string("Enter name of city: ", city_names[i]);
                    if (find_city_index(city_names, i, city_names[i]) != -1) {
                        printf("City name already exists. Please enter a different name.\n");
                        continue;
                    }
                    break;
                }
            }
            for (int i = 0; i < cities_num; ++i) {
                for (int j = 0; j < cities_num; ++j) {
                    if (i == j){
                        graph[i][j].time = 0;
                        graph[i][j].cost = 0;
                        continue;
                    }
                    while (true) {
                        printf("Enter the time from city %s to city %s. ", city_names[i], city_names[j]);
                        if (check_for_float("Enter time(in hours): ", &graph[i][j].time) != 0) {
                            printf("Incorrect input. Please enter an integer.\n");
                            continue;
                        }
                        break;
                    }
                    
                    while (true) {
                        printf("Enter the cost from city %s to city %s. ", city_names[i], city_names[j]);
                        if (check_for_float("Enter cost(UAH): ", &graph[i][j].cost) != 0) {
                            printf("Incorrect input. Please enter an integer.\n");
                            continue;
                        }
                        break;
                    }
                }
            }
            printf("\n                        Detailed routes information       \n");
            printf("_____________________________________________________________________________");
            printf("\n| %-17s| %-17s| %-17s| %-17s|\n", "From City", "To City", "Time(Hours)", "Cost(UAH)");
            printf("|___________________________________________________________________________|\n");
            for (int i = 0; i < cities_num; ++i) {
                for (int j = 0; j < cities_num; ++j) {
                    if (i != j){
                        printf("| %-17s| %-17s| %-17.2f| %-17.2f|\n", city_names[i], city_names[j], graph[i][j].time, graph[i][j].cost);
                    }
                }
            }
            printf("|___________________________________________________________________________|\n");
            while (true){
                int option;
                check_for_int("Do you want to find the optimal route? (1 - Yes, 0 - No): ", &option);
                if (option == 1){
                    int start_city_index, end_city_index;
                    char start_city_name[MAX_CITY_NAME_LENGTH];
                    char end_city_name[MAX_CITY_NAME_LENGTH];

                    // Вводимо назву початкового міста
                    check_for_string("Enter the starting city: ", start_city_name);
                    // Знаходимо індекс початкового міста
                    start_city_index = find_city_index(city_names, cities_num, start_city_name);

                    // Вводимо назву кінцевого міста
                    check_for_string("Enter the ending city: ", end_city_name);
                    // Знаходимо індекс кінцевого міста
                    end_city_index = find_city_index(city_names, cities_num, end_city_name);

                    // Перевіряємо чи міста знайдено
                    if (start_city_index == -1 || end_city_index == -1) {
                        printf("City not found. Please enter a valid city name.\n");
                        continue;
                    }

                    // Викликаємо функцію для знаходження оптимального маршруту
                    optimalRoute(cities_num, graph, start_city_index, end_city_index, city_names);
                }
                else if (option == 0){
                    printf("\n");
                    break;
                }
                else{
                    printf("Incorrect input\n");
                    continue;
                }
            }

            // Free allocated memory for city names
            for (int i = 0; i < cities_num; i++) {
                free(city_names[i]);
            }
            free(city_names);
        }
        else if (choice == 0) {
            // ВИХІД ІЗ ПРОГРАМИ
            system("cls");
            printf("\nEXIT!!!");
            exit(0);
        }
        else {
            printf("Incorrect input\n");
        }
    }
    return 0;
}