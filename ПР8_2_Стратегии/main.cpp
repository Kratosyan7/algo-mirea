// ============================================================================
//  Практическая работа 8_2. Стратегии разработки алгоритмов. Вариант 14.
//
//  Задание 1. Задачи на правила полного перебора (комбинаторика).
//  Задание 2. Задача варианта методом полного перебора (грубой силы):
//             в массиве размером M x N из нулей и единиц найти квадратный
//             блок, состоящий из одних нулей.
//  Задание 3. Та же задача методом ветвей и границ.
//
//  Сборка: g++ -std=c++17 -O2 -Wall main.cpp -o strat
//  Запуск: ./strat 1 | 2 | 3, без аргументов - меню
// ============================================================================

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <algorithm>
#include <cstdio>

using namespace std;
using namespace chrono;

typedef long long ll;
typedef vector<vector<int>> Matrix;

ll steps = 0;            // число проверенных вариантов (шагов перебора)

// ===========================================================================
//  ЗАДАНИЕ 1. Задачи на полный перебор
// ===========================================================================

// Задача 1. Все трёхзначные числа, сумма цифр которых равна 7.
void problem1() {
    cout << "\n--- Задача 1. Трёхзначные числа с суммой цифр 7 ---\n";
    cout << "  обозначения: a - сотни (1..9), b - десятки (0..9), "
         << "c - единицы (0..9)\n"
         << "  алгоритм перебора: a возрастает от 1 до 7, для каждого a "
         << "перебирается b\n  от 0 до 7 - a, тогда c = 7 - a - b "
         << "определяется однозначно\n\n";
    int cnt = 0;
    for (int a = 1; a <= 7; a++) {
        cout << "  a = " << a << ": ";
        for (int b = 0; b <= 7 - a; b++) {
            int c = 7 - a - b;
            cout << a * 100 + b * 10 + c << " ";
            cnt++;
        }
        cout << "\n";
    }
    cout << "\n  всего чисел: " << cnt << "\n";
    cout << "  проверка полным перебором всех трёхзначных чисел: ";
    int cnt2 = 0;
    for (int x = 100; x <= 999; x++)
        if (x / 100 + x / 10 % 10 + x % 10 == 7) cnt2++;
    cout << cnt2 << "\n";
    cout << "\n  количество всех трёхзначных чисел по правилу произведения:\n"
         << "  первая цифра выбирается 9 способами (1..9), вторая и третья "
         << "по 10 способами,\n  следовательно 9 * 10 * 10 = 900 чисел\n";
}

// Задача 2. Из списка {1,2,3,4} выбираются три различных числа a, b, c.
// Сколько способов, чтобы произведение a*(b*c) делилось на 4?
void problem2() {
    cout << "\n--- Задача 2. Тройки из {1,2,3,4}, произведение делится на 4 ---\n";
    vector<int> v = {1, 2, 3, 4};
    int ordered = 0, unordered = 0;
    cout << "  дерево вариантов (перебор упорядоченных троек):\n";
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            if (j == i) continue;
            for (int k = 0; k < 4; k++) {
                if (k == i || k == j) continue;
                int a = v[i], b = v[j], c = v[k];
                int p = a * b * c;
                bool ok = (p % 4 == 0);
                if (ok) ordered++;
                if (i < j && j < k) {
                    cout << "    {" << a << "," << b << "," << c << "} -> "
                         << a << "*" << b << "*" << c << " = " << p
                         << (ok ? "  делится на 4" : "  не делится на 4") << "\n";
                    if (ok) unordered++;
                }
            }
        }
    cout << "\n  сочетаний (порядок не важен), удовлетворяющих условию: "
         << unordered << " из 4\n"
         << "  размещений (порядок важен), удовлетворяющих условию: "
         << ordered << " из 24\n"
         << "  каждое сочетание даёт 3! = 6 упорядоченных троек, поэтому "
         << unordered << " * 6 = " << unordered * 6 << "\n";
}

// Задача 3. Количество нечётных двузначных чисел из цифр 1,3,4,6,7,8,9.
void problem3() {
    cout << "\n--- Задача 3. Нечётные двузначные числа из цифр 1,3,4,6,7,8,9 ---\n";
    vector<int> d = {1, 3, 4, 6, 7, 8, 9};
    cout << "  таблица: строка - первая цифра, столбец - вторая цифра,\n"
         << "  в клетке само число, если оно нечётное\n\n       ";
    for (int y : d) cout << setw(5) << y;
    cout << "\n";
    int cnt = 0;
    for (int x : d) {
        cout << "  " << setw(3) << x << "  ";
        for (int y : d) {
            int num = x * 10 + y;
            if (num % 2 == 1) { cout << setw(5) << num; cnt++; }
            else cout << setw(5) << "-";
        }
        cout << "\n";
    }
    cout << "\n  всего нечётных двузначных чисел: " << cnt << "\n"
         << "  по правилу произведения: первая цифра выбирается 7 способами,\n"
         << "  вторая обязана быть нечётной (1, 3, 7, 9), то есть 4 способа,\n"
         << "  следовательно 7 * 4 = 28 чисел\n";
}

void task1() {
    cout << "\n=== ЗАДАНИЕ 1. Задачи на применение правил полного перебора ===\n";
    problem1();
    problem2();
    problem3();
}

// ===========================================================================
//  Задача варианта: наибольший квадратный блок из нулей в матрице M x N
// ===========================================================================

struct Answer { int size, row, col; };

void printMatrix(const Matrix &a, const Answer &ans = {0, -1, -1}) {
    for (int i = 0; i < (int)a.size(); i++) {
        cout << "    ";
        for (int j = 0; j < (int)a[i].size(); j++) {
            bool in = ans.size > 0 && i >= ans.row && i < ans.row + ans.size &&
                      j >= ans.col && j < ans.col + ans.size;
            cout << (in ? "[" : " ") << a[i][j] << (in ? "]" : " ");
        }
        cout << "\n";
    }
}

// --- метод полного перебора (грубая сила) ----------------------------------
// Перебираются все левые верхние углы и все возможные размеры блока, для
// каждого варианта проверяются все его клетки. Сложность O(M * N * K^3),
// где K = min(M, N).
Answer bruteForce(const Matrix &a) {
    int m = (int)a.size(), n = (int)a[0].size();
    Answer best = {0, -1, -1};
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            for (int s = 1; i + s <= m && j + s <= n; s++) {
                bool ok = true;
                for (int x = i; x < i + s && ok; x++)
                    for (int y = j; y < j + s && ok; y++) {
                        steps++;
                        if (a[x][y] != 0) ok = false;
                    }
                if (!ok) break;                 // больший блок тоже не подойдёт
                if (s > best.size) best = {s, i, j};
            }
    return best;
}

// --- метод ветвей и границ -------------------------------------------------
// Пространство решений - множество всех позиций (i, j). Для каждой позиции
// вычисляется верхняя граница размера блока, который может начаться в этой
// позиции: он не может выйти за границы матрицы, поэтому граница равна
// min(m - i, n - j). Если граница не превышает уже найденного рекорда,
// ветвь отсекается без единой проверки клеток. Дополнительно используется
// матрица длин серий нулей: r[i][j] - количество нулей вправо от (i, j),
// d[i][j] - вниз. Если одна из серий короче рекорда, ветвь также отсекается.
Answer branchAndBound(const Matrix &a) {
    int m = (int)a.size(), n = (int)a[0].size();
    // right[i][j] - количество нулей подряд вправо от клетки (i, j)
    vector<vector<int>> right(m, vector<int>(n + 1, 0));
    for (int i = m - 1; i >= 0; i--)
        for (int j = n - 1; j >= 0; j--)
            right[i][j] = a[i][j] ? 0 : right[i][j + 1] + 1;

    Answer best = {0, -1, -1};
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++) {
            if (a[i][j] != 0) continue;
            // верхняя граница размера блока с углом в (i, j): он не может
            // выйти за нижнюю границу матрицы и не может быть шире серии
            // нулей, начинающейся в этой клетке
            int bound = min(m - i, right[i][j]);
            if (bound <= best.size) continue;        // ветвь отсекается целиком

            // спуск по строкам: minWidth - наименьшая длина серии нулей среди
            // просмотренных строк, поэтому блок из k строк имеет сторону
            // min(k, minWidth)
            int minWidth = right[i][j];
            for (int k = 1; k <= m - i; k++) {
                steps++;
                minWidth = min(minWidth, right[i + k - 1][j]);
                if (minWidth <= best.size && k > best.size) break;  // отсечение
                int side = min(k, minWidth);
                if (side > best.size) best = {side, i, j};
                if (min(m - i, minWidth) <= best.size) break;       // отсечение
            }
        }
    return best;
}

// --- контрольное решение динамическим программированием --------------------
Answer dynamic(const Matrix &a) {
    int m = (int)a.size(), n = (int)a[0].size();
    vector<vector<int>> dp(m, vector<int>(n, 0));
    Answer best = {0, -1, -1};
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++) {
            if (a[i][j] != 0) continue;
            dp[i][j] = (i && j) ? min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]}) + 1 : 1;
            if (dp[i][j] > best.size) best = {dp[i][j], i - dp[i][j] + 1, j - dp[i][j] + 1};
        }
    return best;
}

Matrix randomMatrix(int m, int n, double zeroProb, unsigned seed) {
    mt19937 rnd(seed);
    uniform_real_distribution<double> dist(0, 1);
    Matrix a(m, vector<int>(n));
    for (auto &row : a)
        for (auto &x : row) x = dist(rnd) < zeroProb ? 0 : 1;
    return a;
}

void report(const Matrix &a, const string &title, bool show = true) {
    cout << "\n--- " << title << " ---\n";
    if (show) { cout << "  матрица:\n"; printMatrix(a); }

    steps = 0;
    auto t0 = steady_clock::now();
    Answer b1 = bruteForce(a);
    auto t1 = steady_clock::now();
    ll s1 = steps;
    double tm1 = duration<double, milli>(t1 - t0).count();

    steps = 0;
    t0 = steady_clock::now();
    Answer b2 = branchAndBound(a);
    t1 = steady_clock::now();
    ll s2 = steps;
    double tm2 = duration<double, milli>(t1 - t0).count();

    Answer b3 = dynamic(a);

    cout << "  полный перебор:   размер " << b1.size << ", левый верхний угол ("
         << b1.row << ", " << b1.col << "), проверок клеток " << s1
         << ", время " << fixed << setprecision(4) << tm1 << " мс\n";
    cout << "  ветви и границы:  размер " << b2.size << ", левый верхний угол ("
         << b2.row << ", " << b2.col << "), проверок клеток " << s2
         << ", время " << tm2 << " мс\n";
    cout << "  контроль (динамическое программирование): размер " << b3.size << "\n";
    cout << "  результаты совпадают: "
         << (b1.size == b2.size && b2.size == b3.size ? "да" : "нет") << "\n";
    if (show && b2.size > 0) {
        cout << "  найденный блок отмечен скобками:\n";
        printMatrix(a, b2);
    }
}

void task2() {
    cout << "\n=== ЗАДАНИЕ 2. Метод полного перебора ===\n";
    cout << "  Задача варианта 14: в массиве размером M x N, заполненном "
         << "нулями и\n  единицами, найти квадратный блок, состоящий из одних "
         << "нулей.\n";

    Matrix t1 = {
        {1, 0, 1, 1, 0, 1},
        {0, 0, 0, 0, 1, 0},
        {1, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 1},
        {0, 1, 1, 0, 0, 1}};
    report(t1, "ТЕСТ 1. Матрица 5 x 6, наибольший блок 3 x 3");

    Matrix t2 = {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1}};
    report(t2, "ТЕСТ 2. Нулей нет вовсе");

    Matrix t3 = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}};
    report(t3, "ТЕСТ 3. Все элементы нулевые, блок совпадает с матрицей");

    Matrix t4 = {
        {0, 1, 0, 1, 0},
        {1, 0, 1, 0, 1},
        {0, 1, 0, 1, 0}};
    report(t4, "ТЕСТ 4. Шахматный порядок, наибольший блок 1 x 1");

    Matrix t5 = {
        {1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 1, 1},
        {1, 0, 0, 0, 0, 1, 1},
        {1, 0, 0, 0, 0, 1, 1},
        {1, 0, 0, 0, 0, 1, 1},
        {1, 1, 1, 1, 1, 1, 1}};
    report(t5, "ТЕСТ 5. Блок 4 x 4 внутри матрицы 6 x 7");
}

void task3() {
    cout << "\n=== ЗАДАНИЕ 3. Метод ветвей и границ ===\n";
    cout << "  Сравнение числа проверок и времени на случайных матрицах.\n"
         << "  Доля нулей 0,5; приведено среднее по одной матрице каждого "
         << "размера.\n\n";
    printf("%10s %8s %12s %14s %12s %14s %10s\n", "размер", "ответ",
           "перебор Тф", "перебор, мс", "ветви Тф", "ветви, мс", "выигрыш");
    for (int size : {10, 20, 50, 100, 200, 400}) {
        Matrix a = randomMatrix(size, size, 0.5, 20260930 + size);
        steps = 0;
        auto t0 = steady_clock::now();
        Answer b1 = bruteForce(a);
        auto t1 = steady_clock::now();
        ll s1 = steps;
        double tm1 = duration<double, milli>(t1 - t0).count();

        steps = 0;
        t0 = steady_clock::now();
        Answer b2 = branchAndBound(a);
        t1 = steady_clock::now();
        ll s2 = steps;
        double tm2 = duration<double, milli>(t1 - t0).count();

        printf("%7d x%2d %8d %12lld %14.4f %12lld %14.4f %9.1f\n",
               size, size, b1.size, s1, tm1, s2, tm2, (double)s1 / max(1LL, s2));
        if (b1.size != b2.size) cout << "  ВНИМАНИЕ: ответы не совпали\n";
    }

    cout << "\n  Матрицы с разной долей нулей (размер 200 x 200):\n\n";
    printf("%10s %8s %12s %12s %12s %12s %10s\n", "доля нулей", "ответ",
           "перебор Тф", "перебор, мс", "ветви Тф", "ветви, мс", "выигрыш");
    for (double p : {0.3, 0.5, 0.7, 0.9}) {
        Matrix a = randomMatrix(200, 200, p, 777);
        steps = 0;
        auto q0 = steady_clock::now();
        Answer b1 = bruteForce(a);
        auto q1 = steady_clock::now();
        ll s1 = steps;
        double p1 = duration<double, milli>(q1 - q0).count();
        steps = 0;
        q0 = steady_clock::now();
        Answer b2 = branchAndBound(a);
        q1 = steady_clock::now();
        ll s2 = steps;
        double p2 = duration<double, milli>(q1 - q0).count();
        printf("%10.1f %8d %12lld %12.4f %12lld %12.4f %9.1f\n",
               p, b1.size, s1, p1, s2, p2, (double)s1 / max(1LL, s2));
        if (b1.size != b2.size) cout << "  ВНИМАНИЕ: ответы не совпали\n";
    }
}

int main(int argc, char **argv) {
    ios::sync_with_stdio(0);
    if (argc > 1) {
        string a = argv[1];
        if (a == "1") task1();
        else if (a == "2") task2();
        else if (a == "3") task3();
        return 0;
    }
    while (true) {
        cout << "\n---------------------------------------------\n"
             << " Практическая работа 8_2. Стратегии. Вариант 14.\n"
             << " 1 - задание 1: задачи комбинаторики\n"
             << " 2 - задание 2: полный перебор\n"
             << " 3 - задание 3: ветви и границы\n"
             << " 0 - выход\n выбор: ";
        int c;
        if (!(cin >> c) || c == 0) break;
        if (c == 1) task1(); else if (c == 2) task2(); else if (c == 3) task3();
    }
    return 0;
}
