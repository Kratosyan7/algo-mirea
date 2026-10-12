// ============================================================================
//  Практическая работа 7_2. Алгоритмы на графах. Вариант 14.
//
//  Задание 1. Упражнения: граф отношения x + y <= 7, матрицы смежности,
//             инцидентности и расстояний, вектор удалённости, центр, радиус
//             и диаметр; обходы dfs и bfs заданных графов.
//  Задание 2. Представление графа - список смежных вершин. Задачи варианта:
//             определить глубину графа; определить, является ли граф
//             ациклическим.
//
//  Сборка: g++ -std=c++17 -O2 -Wall main.cpp -o graph
//  Запуск: ./graph 1 - задание 1, ./graph 2 - задание 2, без аргументов - меню
// ============================================================================

#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <stack>
#include <string>
#include <algorithm>

using namespace std;

const int INF = 1000000000;

// ===========================================================================
//  Граф, представленный списком смежных вершин
// ===========================================================================
class Graph {
    int n;                       // количество вершин
    bool directed;               // ориентированный или нет
    vector<vector<int>> adj;     // список смежных вершин

public:
    Graph(int n = 0, bool directed = false) : n(n), directed(directed), adj(n) {}

    int size() const { return n; }
    bool isDirected() const { return directed; }
    const vector<int>& neighbours(int v) const { return adj[v]; }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        if (!directed && u != v) adj[v].push_back(u);
    }

    void sortLists() { for (auto &l : adj) sort(l.begin(), l.end()); }

    void printAdj(int base = 0) const {
        for (int v = 0; v < n; v++) {
            cout << "  " << setw(2) << v + base << ": ";
            for (int u : adj[v]) cout << u + base << " ";
            cout << "\n";
        }
    }

    // ------------------------------------------------------------- матрицы
    vector<vector<int>> adjMatrix() const {
        vector<vector<int>> m(n, vector<int>(n, 0));
        for (int v = 0; v < n; v++)
            for (int u : adj[v]) m[v][u] = 1;
        return m;
    }

    vector<pair<int,int>> edges() const {
        vector<pair<int,int>> e;
        for (int v = 0; v < n; v++)
            for (int u : adj[v])
                if (directed || v <= u) e.push_back({v, u});
        return e;
    }

    // матрица инцидентности: строки - вершины, столбцы - рёбра
    vector<vector<int>> incMatrix() const {
        auto e = edges();
        vector<vector<int>> m(n, vector<int>((int)e.size(), 0));
        for (int j = 0; j < (int)e.size(); j++) {
            auto [u, v] = e[j];
            if (u == v) m[u][j] = 2;             // петля
            else { m[u][j] = 1; m[v][j] = 1; }
        }
        return m;
    }

    // матрица расстояний: кратчайшие пути в рёбрах, обход в ширину из каждой
    // вершины
    vector<vector<int>> distMatrix() const {
        vector<vector<int>> d(n, vector<int>(n, INF));
        for (int s = 0; s < n; s++) {
            queue<int> q;
            q.push(s);
            d[s][s] = 0;
            while (!q.empty()) {
                int v = q.front(); q.pop();
                for (int u : adj[v])
                    if (d[s][u] == INF) { d[s][u] = d[s][v] + 1; q.push(u); }
            }
        }
        return d;
    }

    // ------------------------------------------------------------- обходы
    // обход в глубину: возвращает порядок посещения и массив родителей
    void dfs(int start, vector<int> &order, vector<int> &parent,
             vector<int> &depth) const {
        order.clear();
        parent.assign(n, -1);
        depth.assign(n, -1);
        vector<char> used(n, 0);
        // нерекурсивный обход использует стек
        stack<int> st;
        st.push(start);
        depth[start] = 0;
        while (!st.empty()) {
            int v = st.top(); st.pop();
            if (used[v]) continue;
            used[v] = 1;
            order.push_back(v);
            // соседи помещаются в стек в обратном порядке, чтобы обход шёл
            // по возрастанию номеров вершин
            for (int i = (int)adj[v].size() - 1; i >= 0; i--) {
                int u = adj[v][i];
                if (!used[u]) {
                    parent[u] = v;
                    depth[u] = depth[v] + 1;
                    st.push(u);
                }
            }
        }
    }

    // обход в ширину: порядок посещения, родители и расстояния от start
    void bfs(int start, vector<int> &order, vector<int> &parent,
             vector<int> &dist) const {
        order.clear();
        parent.assign(n, -1);
        dist.assign(n, -1);
        queue<int> q;
        q.push(start);
        dist[start] = 0;
        while (!q.empty()) {
            int v = q.front(); q.pop();
            order.push_back(v);
            for (int u : adj[v])
                if (dist[u] < 0) {
                    dist[u] = dist[v] + 1;
                    parent[u] = v;
                    q.push(u);
                }
        }
    }

    // связность: граф связен, если из любой вершины достижимы все остальные
    bool connected() const {
        vector<int> order, parent, d;
        bfs(0, order, parent, d);
        return (int)order.size() == n;
    }

    // ------------------------------------------- задачи варианта 14
    // Глубина графа: максимальная глубина вершины в дереве обхода в глубину,
    // считая от стартовой вершины. Для несвязного графа обход повторяется от
    // каждой непосещённой вершины, глубина берётся наибольшая.
    int graphDepth() const {
        vector<char> used(n, 0);
        int best = 0;
        for (int s = 0; s < n; s++) {
            if (used[s]) continue;
            vector<int> order, parent, depth;
            dfs(s, order, parent, depth);
            for (int v : order) {
                used[v] = 1;
                best = max(best, depth[v]);
            }
        }
        return best;
    }

    // Ацикличность. Для неориентированного графа цикл обнаруживается, если
    // при обходе встретилась уже посещённая вершина, не являющаяся родителем
    // текущей. Для ориентированного используется раскраска вершин: серая
    // вершина на пути означает обратное ребро, то есть цикл.
    bool acyclicUndirected(vector<int> &cycle) const {
        vector<int> parent(n, -2);
        for (int s = 0; s < n; s++) {
            if (parent[s] != -2) continue;
            stack<pair<int,int>> st;             // вершина и её родитель
            st.push({s, -1});
            parent[s] = -1;
            while (!st.empty()) {
                auto [v, p] = st.top(); st.pop();
                for (int u : adj[v]) {
                    if (u == v) { cycle = {v, v}; return false; }   // петля
                    if (parent[u] == -2) {
                        parent[u] = v;
                        st.push({u, v});
                    } else if (u != p) {
                        cycle = {v, u};
                        return false;
                    }
                }
            }
        }
        return true;
    }

    bool acyclicDirected(vector<int> &cycle) const {
        vector<int> color(n, 0);                 // 0 белая, 1 серая, 2 чёрная
        vector<int> parent(n, -1);
        bool ok = true;
        // рекурсивный обход в глубину с раскраской
        function<void(int)> go = [&](int v) {
            color[v] = 1;
            for (int u : adj[v]) {
                if (!ok) return;
                if (color[u] == 0) { parent[u] = v; go(u); }
                else if (color[u] == 1) { cycle = {v, u}; ok = false; return; }
            }
            color[v] = 2;
        };
        for (int v = 0; v < n && ok; v++)
            if (color[v] == 0) go(v);
        return ok;
    }

    bool acyclic(vector<int> &cycle) const {
        return directed ? acyclicDirected(cycle) : acyclicUndirected(cycle);
    }
};

#include <functional>

// ---------------------------------------------------------------------------
//  Печать матриц
// ---------------------------------------------------------------------------
void printMatrix(const vector<vector<int>> &m, const string &title,
                 int base = 0, bool dist = false) {
    cout << "\n  " << title << "\n      ";
    for (size_t j = 0; j < m[0].size(); j++) cout << setw(3) << j + base;
    cout << "\n";
    for (size_t i = 0; i < m.size(); i++) {
        cout << "  " << setw(3) << i + base << " ";
        for (size_t j = 0; j < m[i].size(); j++) {
            if (dist && m[i][j] >= INF) cout << setw(3) << "-";
            else cout << setw(3) << m[i][j];
        }
        cout << "\n";
    }
}

// ===========================================================================
//  ЗАДАНИЕ 1. Упражнения
// ===========================================================================
void task1() {
    cout << "\n=== ЗАДАНИЕ 1. Упражнения ===\n";

    // --- граф отношения x + y <= 7 на вершинах 1..6 ------------------------
    cout << "\n--- Граф G отношения x + y <= 7, X = {1, 2, 3, 4, 5, 6} ---\n";
    Graph g(6, false);                    // вершины 0..5 соответствуют 1..6
    cout << "  пары (x, y), удовлетворяющие условию:\n   ";
    for (int x = 1; x <= 6; x++)
        for (int y = x; y <= 6; y++)
            if (x + y <= 7) {
                g.addEdge(x - 1, y - 1);
                cout << " (" << x << "," << y << ")";
            }
    cout << "\n";
    g.sortLists();
    cout << "\n  список смежных вершин:\n";
    g.printAdj(1);
    cout << "  отношение симметрично (x + y = y + x), поэтому граф "
         << "неориентированный.\n  Пары (1,1), (2,2), (3,3) дают петли.\n";

    printMatrix(g.adjMatrix(), "матрица смежности:", 1);

    auto e = g.edges();
    cout << "\n  рёбра графа:";
    for (size_t j = 0; j < e.size(); j++)
        cout << " e" << j + 1 << "=(" << e[j].first + 1 << "," << e[j].second + 1 << ")";
    cout << "\n";
    auto inc = g.incMatrix();
    cout << "\n  матрица инцидентности (строки - вершины, столбцы - рёбра):\n      ";
    for (size_t j = 0; j < e.size(); j++) cout << setw(4) << "e" + to_string(j + 1);
    cout << "\n";
    for (int i = 0; i < 6; i++) {
        cout << "  " << setw(3) << i + 1 << " ";
        for (size_t j = 0; j < e.size(); j++) cout << setw(4) << inc[i][j];
        cout << "\n";
    }

    auto d = g.distMatrix();
    printMatrix(d, "матрица расстояний:", 1, true);

    cout << "\n  вектор удалённости d(i) = max по j расстояния d(i, j):\n   ";
    vector<int> ecc(6);
    for (int i = 0; i < 6; i++) {
        ecc[i] = 0;
        for (int j = 0; j < 6; j++) if (d[i][j] < INF) ecc[i] = max(ecc[i], d[i][j]);
        cout << "  d(" << i + 1 << ") = " << ecc[i];
    }
    int radius = *min_element(ecc.begin(), ecc.end());
    int diameter = *max_element(ecc.begin(), ecc.end());
    cout << "\n\n  радиус графа  r = min d(i) = " << radius << "\n";
    cout << "  диаметр графа D = max d(i) = " << diameter << "\n";
    cout << "  центр графа (вершины с d(i) = r):";
    for (int i = 0; i < 6; i++) if (ecc[i] == radius) cout << " " << i + 1;
    cout << "\n";

    // --- граф для обхода в глубину -----------------------------------------
    cout << "\n--- Граф H1: 3-7, 1-4, 7-8, 0-5, 5-2, 3-8, 2-9, 0-6, 4-9, 2-6, 6-4 ---\n";
    Graph h1(10, false);
    vector<pair<int,int>> e1 = {{3,7},{1,4},{7,8},{0,5},{5,2},{3,8},{2,9},
                               {0,6},{4,9},{2,6},{6,4}};
    for (auto [u, v] : e1) h1.addEdge(u, v);
    h1.sortLists();
    cout << "  список смежных вершин:\n";
    h1.printAdj();
    printMatrix(h1.adjMatrix(), "матрица смежности:");

    cout << "\n  связность: ";
    vector<int> order, parent, depth;
    h1.bfs(0, order, parent, depth);
    cout << (h1.connected() ? "граф связный" : "граф несвязный")
         << ", из вершины 0 достижимо " << order.size() << " вершин из 10\n";

    h1.dfs(0, order, parent, depth);
    cout << "\n  обход в глубину из вершины 0:\n   ";
    for (int v : order) cout << " " << v;
    cout << "\n  дерево обхода (вершина: родитель, глубина):\n";
    for (int v : order)
        cout << "    " << v << ": родитель "
             << (parent[v] < 0 ? string("нет") : to_string(parent[v]))
             << ", глубина " << depth[v] << "\n";

    // --- граф для обхода в ширину ------------------------------------------
    cout << "\n--- Граф H2: 0-3, 0-7, 0-5, 4-7, 3-6, 1-2, 1-5, 1-6, 1-7, 2-5 ---\n";
    Graph h2(8, false);
    vector<pair<int,int>> e2 = {{0,3},{0,7},{0,5},{4,7},{3,6},{1,2},{1,5},
                               {1,6},{1,7},{2,5}};
    for (auto [u, v] : e2) h2.addEdge(u, v);
    h2.sortLists();
    cout << "  список смежных вершин:\n";
    h2.printAdj();
    printMatrix(h2.adjMatrix(), "матрица смежности:");

    vector<int> dist;
    h2.bfs(0, order, parent, dist);
    cout << "\n  связность: " << (h2.connected() ? "граф связный" : "граф несвязный")
         << "\n";
    cout << "  обход в ширину из вершины 0:\n   ";
    for (int v : order) cout << " " << v;
    cout << "\n\n  таблица обхода:\n";
    cout << "  вершина           ";
    for (int v = 0; v < 8; v++) cout << setw(4) << v;
    cout << "\n  порядок посещения ";
    vector<int> pos(8, -1);
    for (int i = 0; i < (int)order.size(); i++) pos[order[i]] = i;
    for (int v = 0; v < 8; v++) cout << setw(4) << pos[v];
    cout << "\n  родитель вершины  ";
    for (int v = 0; v < 8; v++)
        if (parent[v] < 0) cout << setw(4) << "-"; else cout << setw(4) << parent[v];
    cout << "\n  расстояние от 0   ";
    for (int v = 0; v < 8; v++) cout << setw(4) << dist[v];
    cout << "\n";

    cout << "\n  кратчайший путь от вершины 0 к вершине 2: ";
    vector<int> path;
    for (int v = 2; v >= 0; v = parent[v]) { path.push_back(v); if (v == 0) break; }
    reverse(path.begin(), path.end());
    for (size_t i = 0; i < path.size(); i++) cout << (i ? " -> " : "") << path[i];
    cout << ", длина " << dist[2] << " ребра\n";
}

// ===========================================================================
//  ЗАДАНИЕ 2. Задачи варианта: глубина графа и ацикличность
// ===========================================================================
void solve(Graph &g, const string &title, int base = 0) {
    cout << "\n--- " << title << " ---\n";
    cout << "  список смежных вершин:\n";
    g.printAdj(base);
    cout << "  глубина графа: " << g.graphDepth() << "\n";
    vector<int> cycle;
    bool ok = g.acyclic(cycle);
    cout << "  граф " << (ok ? "ациклический" : "содержит цикл");
    if (!ok) cout << " (обнаружен на ребре " << cycle[0] + base << " - "
                  << cycle[1] + base << ")";
    cout << "\n";
}

void task2() {
    cout << "\n=== ЗАДАНИЕ 2. Глубина графа и проверка ацикличности ===\n";

    Graph t1(7, false);
    for (auto [u, v] : vector<pair<int,int>>{{0,1},{0,2},{1,3},{1,4},{2,5},{5,6}})
        t1.addEdge(u, v);
    t1.sortLists();
    solve(t1, "ТЕСТ 1. Неориентированное дерево из 7 вершин (циклов нет)");

    Graph t2(5, false);
    for (auto [u, v] : vector<pair<int,int>>{{0,1},{1,2},{2,3},{3,0},{3,4}})
        t2.addEdge(u, v);
    t2.sortLists();
    solve(t2, "ТЕСТ 2. Неориентированный граф с циклом 0-1-2-3-0");

    Graph t3(6, false);
    for (auto [u, v] : vector<pair<int,int>>{{0,1},{1,2},{3,4}})
        t3.addEdge(u, v);
    t3.sortLists();
    solve(t3, "ТЕСТ 3. Несвязный граф: две компоненты и изолированная вершина");

    Graph t4(6, true);
    for (auto [u, v] : vector<pair<int,int>>{{0,1},{1,2},{2,3},{0,4},{4,5}})
        t4.addEdge(u, v);
    solve(t4, "ТЕСТ 4. Ориентированный ациклический граф");

    Graph t5(4, true);
    for (auto [u, v] : vector<pair<int,int>>{{0,1},{1,2},{2,3},{3,1}})
        t5.addEdge(u, v);
    solve(t5, "ТЕСТ 5. Ориентированный граф с циклом 1 -> 2 -> 3 -> 1");

    Graph t6(4, false);
    t6.addEdge(0, 0);
    t6.addEdge(1, 2);
    solve(t6, "ТЕСТ 6. Граф с петлёй в вершине 0");

    cout << "\n--- Графы из таблицы 18 методических указаний ---\n";

    // граф табл. 18: ориентированный, вершины 1..6 (в программе 0..5)
    Graph g18a(6, true);
    for (auto [u, v] : vector<pair<int,int>>{{0,1},{0,2},{1,4},{1,3},{2,4},
         {2,3},{4,5},{3,5}}) g18a.addEdge(u, v);
    solve(g18a, "Граф 1 из табл. 18 (ориентированный, вершины 1..6)", 1);

    // граф табл. 18: неориентированный, вершины 1..6
    Graph g18b(6, false);
    for (auto [u, v] : vector<pair<int,int>>{{0,5},{0,4},{4,5},{5,1},{5,2},
         {5,3},{1,2},{2,3}}) g18b.addEdge(u, v);
    g18b.sortLists();
    solve(g18b, "Граф 2 из табл. 18 (неориентированный, вершины 1..6)", 1);

    cout << "\n--- Графы задания 1 ---\n";
    Graph h1(10, false);
    for (auto [u, v] : vector<pair<int,int>>{{3,7},{1,4},{7,8},{0,5},{5,2},
         {3,8},{2,9},{0,6},{4,9},{2,6},{6,4}}) h1.addEdge(u, v);
    h1.sortLists();
    solve(h1, "Граф H1 из задания 1");

    Graph h2(8, false);
    for (auto [u, v] : vector<pair<int,int>>{{0,3},{0,7},{0,5},{4,7},{3,6},
         {1,2},{1,5},{1,6},{1,7},{2,5}}) h2.addEdge(u, v);
    h2.sortLists();
    solve(h2, "Граф H2 из задания 1");
}

int main(int argc, char **argv) {
    ios::sync_with_stdio(0);
    if (argc > 1) {
        string a = argv[1];
        if (a == "1") task1();
        else if (a == "2") task2();
        return 0;
    }
    while (true) {
        cout << "\n---------------------------------------------\n"
             << " Практическая работа 7_2. Графы. Вариант 14.\n"
             << " 1 - задание 1: упражнения на графах\n"
             << " 2 - задание 2: глубина графа и ацикличность\n"
             << " 0 - выход\n выбор: ";
        int c;
        if (!(cin >> c) || c == 0) break;
        if (c == 1) task1();
        else if (c == 2) task2();
    }
    return 0;
}
