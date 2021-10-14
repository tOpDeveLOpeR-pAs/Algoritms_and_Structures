#include <iostream>
#include <tuple>
#include <deque>


const size_t SIZE = 5;                              // количество вершин графа
const int MAX_VALUE = 10000;                        // максимальное значение веса

std::tuple<int*, int*> Dijkstra(int weights[][SIZE], size_t root) {
    static int short_lens[SIZE];
    static int parents[SIZE];
    bool visited[SIZE];

    // Инициализация
    for (size_t i = 0; i < SIZE; ++i) {
        short_lens[i] = MAX_VALUE;
        parents[i] = -1;                             // 0 - без родителя
        visited[i] = false;
    }

    short_lens[root] = 0;                           // начальное значение для стартовой вершины

    // Основной алгоритм
    int min_len;                                    // регистр для хранения минимального расстояния
    size_t index = 0;                               // индекс текущей вершины

    for (size_t i = 0; i < SIZE; ++i) {
        // Ищем самую близкую вершину
        min_len = MAX_VALUE;
        for (size_t j = 0; j < SIZE; ++j) {
            // Если вершина не посещена и расстояние до нее ближе, уже найденного..
            if (!visited[j] && short_lens[j] < min_len) {
                min_len = short_lens[j];
                index = j;
            }
        }

        visited[index] = true;                      // считаем теперь вершину посещенной
        // для всех ее соседей выполняем релаксацию
        for (size_t j = 0; j < SIZE; ++j) {
            if (!visited[j] && short_lens[j] > short_lens[index] + weights[index][j]
                            && weights[index][j] != MAX_VALUE) {
                short_lens[j] = short_lens[index] + weights[index][j];
                parents[j] = static_cast<int>(index);
            }
        }
    }

    return std::make_tuple(short_lens, parents);
};

void SearchPath(const int* parents, size_t end, std::deque<int>* path) {
    int curr_v = parents[end];
    while (curr_v != -1) {
        path->push_front(curr_v);
        curr_v = parents[curr_v];
    }
};

int main() {
    // задаем массив весов / таблица смежности
    int weights[SIZE][SIZE] = {{0, 2, 3, 4, MAX_VALUE},
                               {MAX_VALUE, 0, MAX_VALUE, 1, 90},
                               {MAX_VALUE, MAX_VALUE, 0, 1, 7},
                               {MAX_VALUE, MAX_VALUE, MAX_VALUE, 0, 1},
                               {MAX_VALUE, MAX_VALUE, MAX_VALUE, MAX_VALUE, 0}};

    // Дейкстра -> массив кратчайших путей, массив предков для каждой вершины
    auto [short_lens, parents] = Dijkstra(weights, 0);

    // Кратчайший путь для заданной вершины по полученным данным Дейкстра
    std::deque<int> path;
    SearchPath(parents, 4, &path);

/*  for (size_t i = 0; i < SIZE; ++i) {
        std::cout << short_lens[i] << " ";
    }

    std::cout << "\n";
    for (size_t i = 0; i < SIZE; ++i) {
        std::cout << parents[i] << " ";
    }

    std::cout << "\n";
    for (auto it: path) {
        std::cout << it;
    }*/

    path.clear();
    path.shrink_to_fit();

    return 0;
}
