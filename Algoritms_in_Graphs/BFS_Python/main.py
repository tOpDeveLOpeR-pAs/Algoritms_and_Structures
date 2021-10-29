from collections import deque


# алгоритм поиска в ширину -> возвращаем словарь из длин до root, и список из родителей
def bfs(graph: dict[list], root: int) -> (dict, dict):
    visited, vertices = set(), deque()
    lens, parents = dict(), dict()

    # начальная инициализация - O(|V|)
    for v in graph.keys():
        lens[v] = -1            # нельзя добраться
        parents[v] = -1         # нет родителя

    vertices.append(root)
    lens[root] = 0

    # работа алгоритма - O(|E|)
    while len(vertices) > 0:
        curr_v = vertices.popleft()
        for child in graph[curr_v]:
            if child not in visited:
                parents[child] = curr_v
                lens[child] = lens[curr_v] + 1
                vertices.append(child)
                visited.add(child)
        visited.add(curr_v)

    return lens, parents


if __name__ == '__main__':
    graph = {0: [1, 2, 3], 1: [2], 2: [3], 3: [1,2]}
    lens, parents = bfs(graph, 0)

    print(lens)
    print(parents)

