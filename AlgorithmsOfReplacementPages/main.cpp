#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <vector>
#include <algorithm>


// реализация алгоритма замещения FIFO
std::pair<int, std::vector<std::pair<int, bool>>> replacement_FIFO(std::vector<int>& pages, int capacity) {
    std::unordered_set<int> frames;                          // множество страниц, лежащих в фреймах
    std::queue<int> indexes;                                 // для реализации FIFO
    std::vector<std::pair<int, bool>> pages_in;              // второй компонент, возвращаемой пары

    int pages_fault = 0;
    for (int & page : pages) {
        bool is_not_in = frames.find(page) == frames.end();     // лежит ли страница в фреймах?

        // если есть незаполненные фреймы
        if (frames.size() < static_cast<size_t>(capacity) && is_not_in) {
            indexes.push(page);
            frames.insert(page);
            ++pages_fault;
        }
        else if (is_not_in) {
            int rm_page = indexes.front();
            indexes.pop();

            frames.erase(rm_page);
            frames.insert(page);

            indexes.push(page);
            ++pages_fault;
        }
        pages_in.emplace_back(page, !is_not_in);
    }
    return std::make_pair(pages_fault, pages_in);
}


// реализация алгоритма замещения LRU
std::pair<int, std::vector<std::pair<int, bool>>> replacement_LRU(std::vector<int>& pages, int capacity) {
    std::unordered_set<int> frames;                          // множество страниц, лежащих в фреймах
    std::unordered_map<int, int> indexes;                    // пары вида <страница, последняя запись>
    std::vector<std::pair<int, bool>> pages_in;              // второй компонент, возвращаемой пары

    int pages_fault = 0;
    for (int i = 0; i < static_cast<int>(pages.size()); ++i) {
        bool is_not_in = frames.find(pages[i]) == frames.end();     // лежит ли страница в фреймах?

        // если есть свободные фреймы
        if (frames.size() < static_cast<size_t>(capacity) && is_not_in) {
            frames.insert(pages[i]);
            ++pages_fault;
        }
        else if (is_not_in) {
            // поиск наименее используемой страницы
            auto page_index = indexes.begin();
            auto page_smaller_i = page_index++;     // наименне используемая страница / ее индекс использования
            for (; page_index != indexes.end(); ++page_index) {
                if (page_index->second < page_smaller_i->second) {
                    page_smaller_i = page_index;
                }
            }

            // удаляем из фреймов
            frames.erase(page_smaller_i->first);
            // вставка новой страницы во фреймы
            frames.insert(pages[i]);

            // удаляем этот элемент из компонент, лежащих во фреймах
            indexes.erase(page_smaller_i->first);

            ++pages_fault;
        }

        indexes[pages[i]] = i;                                          // обновление компонент, лежащих во фрейме
        pages_in.emplace_back(pages[i], !is_not_in);
    }

    return std::make_pair(pages_fault, pages_in);
}


// алгоритм замещения страниц LFU
std::pair<int, std::vector<std::pair<int, bool>>> replacement_LFU(std::vector<int>& pages, int capacity) {
    std::vector<int> frames;                                 // множество страниц, лежащих в фреймах, для сохранения индекса
    std::unordered_map<int, int> page_frequency;             // пары вида <страница, частота>
    std::vector<std::pair<int, bool>> pages_in;              // второй компонент, возвращаемой пары

    int pages_fault = 0;
    for (int & page : pages) {
        auto it = std::find(frames.begin(), frames.end(), page);
        bool is_not_in = (it == frames.end());

        // если страница не лежит во фреймах
        if (is_not_in) {
            if (frames.size() == static_cast<size_t>(capacity)) {
                --page_frequency[frames[0]];                 // уменьшаем частоту наименее часто встречающейся страницы
                frames.erase(frames.begin());
            }

            frames.push_back(page);                      // добавляем в текущий кеш с учетом индекса
            ++page_frequency[page];                      // увеличиваем частоту
            ++pages_fault;
        }
        else {
            // увеличиваем частоту
            ++page_frequency[page];

            // смена индекса
            frames.erase(it);
            frames.push_back(page);
        };

        // сортировка frames по частоте страниц // при совпадении -> замена - с меньшим номером
        int k = static_cast<int>(frames.size()) - 2;
        while (page_frequency[frames[k]] >= page_frequency[frames[k + 1]] && k >= 0) {
            if (page_frequency[frames[k]] > page_frequency[frames[k + 1]] || frames[k] > frames[k+1]) {
                std::swap(frames[k + 1], frames[k]);
            }
            --k;
        }

        pages_in.emplace_back(page, !is_not_in);
    }

    return std::make_pair(pages_fault, pages_in);
}


// алгоритм замещения страниц OPT
std::pair<int, std::vector<std::pair<int, bool>>> replacement_OPT(std::vector<int>& pages, int capacity) {
    std::vector<int> frames;                                 // множество страниц, лежащих в фреймах, для сохранения индекса
    std::vector<std::pair<int, bool>> pages_in;              // второй компонент, возвращаемой пары

    int hit = 0;                                             // счетчик обращений
    for (int i = 0; i < static_cast<int>(pages.size()); ++i) {
        bool is_in = (std::find(frames.begin(), frames.end(), pages[i]) != frames.end());
        pages_in.emplace_back(pages[i], is_in);

        // если лежит во фреймах
        if (is_in) {
            ++hit;
            continue;
        }

        // если фреймы не заполнены
        if (frames.size() < static_cast<size_t>(capacity)) {
            frames.push_back(pages[i]);
        }
        else {
            // предсказываем самую дальнюю по обращению страницу
            int res = -1;
            int index = i + 1;
            int farthest = index;
            for (int k1 = 0; k1 < static_cast<int>(frames.size()); ++k1) {
                int k2;
                for (k2 = index; k2 < static_cast<int>(pages.size()); ++k2) {
                    if (frames[k1] == pages[k2]) {
                        if (k2 > farthest) {
                            farthest = k2;
                            res = k1;
                        }
                        break;
                    }
                }

                // если больше не использовалась в будущем
                if (k2 == static_cast<int>(pages.size())) {
                    res = k1;
                    break;
                }
            }
            res = (res == -1 ? 0 : res);
            // замена
            frames[res] = pages[i];
        }
    }

    return std::make_pair(pages.size() - hit, pages_in);
}

int main() {
    int capacity, num;                  // количество фреймов, количество запросов
    std::cin >> capacity >> num;

    std::vector<int> pages(num);
    for (auto& page: pages) std::cin >> page;

    // результаты работы алгоритмов замещения в виде < page_faults, очередь с <страница, is_in> >
    std::pair<int, std::vector<std::pair<int, bool>>> result_FIFO = replacement_FIFO(pages, capacity);
    std::pair<int, std::vector<std::pair<int, bool>>> result_LRU = replacement_LRU(pages, capacity);
    std::pair<int, std::vector<std::pair<int, bool>>> result_LFU = replacement_LFU(pages, capacity);
    std::pair<int, std::vector<std::pair<int, bool>>> result_OPT = replacement_OPT(pages, capacity);

    // вывод очередей работы алгоритмов по порядку: FIFO, LRU, LFU, OPT
    for (const auto& page_in: result_FIFO.second) {
        std::cout << page_in.first << (page_in.second ? "+" : "-") << " ";
    }
    std::cout << "\n";
    for (const auto& page_in: result_LRU.second) {
        std::cout << page_in.first << (page_in.second ? "+" : "-") << " ";
    }
    std::cout << "\n";
    for (const auto& page_in: result_LFU.second) {
        std::cout << page_in.first << (page_in.second ? "+" : "-") << " ";
    }
    std::cout << "\n";
    for (const auto& page_in: result_OPT.second) {
        std::cout << page_in.first << (page_in.second ? "+" : "-") << " ";
    }

    // вывод ошибок страниц для каждого алгоритма по ананличному порядку
    std::cout << "\n";
    std::cout << result_FIFO.first << " ";
    std::cout << result_LRU.first << " ";
    std::cout << result_LFU.first << " ";
    std::cout << result_OPT.first << " ";
}
