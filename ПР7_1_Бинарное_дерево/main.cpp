// ============================================================================
//  Практическая работа 7_1. Нелинейные структуры данных. Бинарное дерево.
//  Вариант 14: информационная часть узла - целое число.
//  Операции варианта:
//    1) определить, сколько узлов дерева содержат заданное значение;
//    2) определить, является ли дерево полным;
//    3) найти максимальное значение, хранящееся в узлах дерева.
//
//  Вид дерева: идеально сбалансированное бинарное дерево (ИСБД) из n узлов,
//  построенное из случайной последовательности ключей.
//
//  Сборка: g++ -std=c++17 -O2 -Wall main.cpp -o tree
//  Запуск: ./tree       - текстовое меню
//          ./tree demo  - демонстрация всех операций на тестах
// ============================================================================

#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <string>
#include <random>
#include <algorithm>

using namespace std;

typedef long long ll;

// ---------------------------------------------------------------------------
//  Узел дерева: информационная часть и два указателя на поддеревья
// ---------------------------------------------------------------------------
struct Node {
    int value;
    Node *left, *right;
    Node(int v) : value(v), left(nullptr), right(nullptr) {}
};

// ===========================================================================
//  Класс управления идеально сбалансированным бинарным деревом
// ===========================================================================
class BinTree {
    Node *root;

    // рекурсивное построение ИСБД из n узлов: nl = n / 2 узлов уходит в левое
    // поддерево, один узел становится корнем, остальные в правое поддерево
    Node* build(int n, const vector<int> &keys, int &pos) {
        if (n == 0) return nullptr;
        int nl = n / 2, nr = n - nl - 1;
        Node *p = new Node(keys[pos++]);
        p->left = build(nl, keys, pos);
        p->right = build(nr, keys, pos);
        return p;
    }

    void clear(Node *p) {
        if (!p) return;
        clear(p->left);
        clear(p->right);
        delete p;
    }

    // вывод дерева, повёрнутого на правый бок: правое поддерево, корень, левое
    void show(Node *p, int level) const {
        if (!p) return;
        show(p->right, level + 1);
        cout << string(level * 4, ' ') << p->value << "\n";
        show(p->left, level + 1);
    }

    int height(Node *p) const {
        if (!p) return 0;
        return 1 + max(height(p->left), height(p->right));
    }

    int nodes(Node *p) const {
        if (!p) return 0;
        return 1 + nodes(p->left) + nodes(p->right);
    }

    // операция 1 варианта: количество узлов с заданным значением
    int countValue(Node *p, int x) const {
        if (!p) return 0;
        return (p->value == x) + countValue(p->left, x) + countValue(p->right, x);
    }

    // операция 2 варианта: полное дерево - каждый узел имеет либо два
    // поддерева, либо ни одного
    bool isFull(Node *p) const {
        if (!p) return true;
        if (!p->left && !p->right) return true;
        if (p->left && p->right) return isFull(p->left) && isFull(p->right);
        return false;
    }

    // совершенное дерево: полное и все листья на одном уровне
    bool isPerfect(Node *p, int h, int level) const {
        if (!p) return true;
        if (!p->left && !p->right) return level + 1 == h;
        if (!p->left || !p->right) return false;
        return isPerfect(p->left, h, level + 1) && isPerfect(p->right, h, level + 1);
    }

    // операция 3 варианта: максимальное значение в узлах дерева
    int findMax(Node *p) const {
        int mx = p->value;
        if (p->left)  mx = max(mx, findMax(p->left));
        if (p->right) mx = max(mx, findMax(p->right));
        return mx;
    }

    void inorder(Node *p, vector<int> &out) const {
        if (!p) return;
        inorder(p->left, out);
        out.push_back(p->value);
        inorder(p->right, out);
    }

public:
    BinTree() : root(nullptr) {}
    ~BinTree() { clear(root); }

    // создать ИСБД из последовательности ключей
    void create(const vector<int> &keys) {
        clear(root);
        int pos = 0;
        root = build((int)keys.size(), keys, pos);
    }

    bool empty() const { return root == nullptr; }
    int  size()   const { return nodes(root); }
    int  height() const { return height(root); }

    void show() const {
        if (!root) { cout << "  дерево пусто\n"; return; }
        show(root, 0);
    }

    int  countValue(int x) const { return countValue(root, x); }
    int  findMax()         const { return findMax(root); }
    bool isFull()          const { return isFull(root); }
    bool isPerfect()       const { return isPerfect(root, height(root), 0); }

    // завершённое дерево: все уровни, кроме последнего, заполнены, а
    // последний заполнен слева направо. Проверяется обходом в ширину:
    // после первого пустого потомка не должно встретиться ни одного узла
    bool isComplete() const {
        if (!root) return true;
        queue<Node*> q;
        q.push(root);
        bool seenGap = false;
        while (!q.empty()) {
            Node *p = q.front(); q.pop();
            if (!p) { seenGap = true; continue; }
            if (seenGap) return false;
            q.push(p->left);
            q.push(p->right);
        }
        return true;
    }

    // обход в ширину, для наглядного вывода по уровням
    vector<vector<int>> levels() const {
        vector<vector<int>> res;
        if (!root) return res;
        queue<Node*> q;
        q.push(root);
        while (!q.empty()) {
            int sz = (int)q.size();
            vector<int> cur;
            while (sz--) {
                Node *p = q.front(); q.pop();
                cur.push_back(p->value);
                if (p->left)  q.push(p->left);
                if (p->right) q.push(p->right);
            }
            res.push_back(cur);
        }
        return res;
    }

    vector<int> inorder() const {
        vector<int> out;
        inorder(root, out);
        return out;
    }
};

// ---------------------------------------------------------------------------
//  Вспомогательные функции
// ---------------------------------------------------------------------------
vector<int> randomKeys(int n, int lo = 1, int hi = 99) {
    static mt19937 rnd(20260930);
    uniform_int_distribution<int> dist(lo, hi);
    vector<int> keys(n);
    for (auto &k : keys) k = dist(rnd);
    return keys;
}

void printKeys(const vector<int> &keys) {
    cout << "  входной поток ключей:";
    for (int k : keys) cout << " " << k;
    cout << "\n";
}

void report(BinTree &t, int searchValue) {
    cout << "\n  структура дерева (дерево повёрнуто на правый бок):\n";
    t.show();
    cout << "\n  узлов: " << t.size() << ", высота: " << t.height() << "\n";
    cout << "  по уровням:\n";
    auto lv = t.levels();
    for (size_t i = 0; i < lv.size(); i++) {
        cout << "    уровень " << i << ":";
        for (int v : lv[i]) cout << " " << v;
        cout << "\n";
    }
    cout << "\n  ОПЕРАЦИЯ 1. Узлов со значением " << searchValue << ": "
         << t.countValue(searchValue) << "\n";
    cout << "  ОПЕРАЦИЯ 2. Дерево полное (каждый узел имеет 0 или 2 потомка): "
         << (t.isFull() ? "да" : "нет") << "\n";
    cout << "              дополнительно: совершенное (все листья на одном "
         << "уровне): " << (t.isPerfect() ? "да" : "нет") << "\n";
    cout << "              дополнительно: завершённое (последний уровень "
         << "заполнен слева): " << (t.isComplete() ? "да" : "нет") << "\n";
    cout << "  ОПЕРАЦИЯ 3. Максимальное значение в дереве: " << t.findMax() << "\n";
}

// ---------------------------------------------------------------------------
//  Демонстрация на тестовых примерах
// ---------------------------------------------------------------------------
void demo() {
    BinTree t;

    cout << "\n=== ТЕСТ 1. ИСБД из 7 узлов (2^3 - 1, дерево совершенное) ===\n";
    vector<int> k1 = {15, 8, 23, 42, 4, 16, 8};
    printKeys(k1);
    t.create(k1);
    report(t, 8);

    cout << "\n=== ТЕСТ 2. ИСБД из 10 узлов (последний уровень неполный) ===\n";
    vector<int> k2 = {31, 7, 19, 7, 55, 12, 7, 60, 25, 3};
    printKeys(k2);
    t.create(k2);
    report(t, 7);

    cout << "\n=== ТЕСТ 3. Дерево из одного узла ===\n";
    vector<int> k3 = {42};
    printKeys(k3);
    t.create(k3);
    report(t, 42);

    cout << "\n=== ТЕСТ 4. Пустое дерево ===\n";
    t.create({});
    cout << "  узлов: " << t.size() << "\n";
    t.show();

    cout << "\n=== ТЕСТ 5. ИСБД из 12 случайных ключей ===\n";
    vector<int> k5 = randomKeys(12);
    printKeys(k5);
    t.create(k5);
    report(t, k5[0]);

    cout << "\n=== ТЕСТ 6. Значение, которого нет в дереве ===\n";
    cout << "  узлов со значением 1000: " << t.countValue(1000) << "\n";
}

int main(int argc, char **argv) {
    ios::sync_with_stdio(0);
    if (argc > 1 && string(argv[1]) == "demo") { demo(); return 0; }

    BinTree t;
    vector<int> keys;
    while (true) {
        cout << "\n---------------------------------------------\n"
             << " Практическая работа 7_1. Бинарное дерево. Вариант 14.\n"
             << " 1 - создать ИСБД из n случайных ключей\n"
             << " 2 - создать ИСБД из введённых ключей\n"
             << " 3 - вывести структуру дерева\n"
             << " 4 - количество узлов с заданным значением\n"
             << " 5 - является ли дерево полным\n"
             << " 6 - максимальное значение в дереве\n"
             << " 7 - демонстрация на тестовых примерах\n"
             << " 0 - выход\n выбор: ";
        int c;
        if (!(cin >> c) || c == 0) break;
        if (c == 1) {
            int n; cout << " n = "; cin >> n;
            keys = randomKeys(n);
            printKeys(keys);
            t.create(keys);
            t.show();
        } else if (c == 2) {
            int n; cout << " n = "; cin >> n;
            keys.assign(n, 0);
            cout << " ключи: ";
            for (auto &k : keys) cin >> k;
            t.create(keys);
            t.show();
        } else if (c == 3) {
            t.show();
        } else if (c == 4) {
            if (t.empty()) { cout << " дерево пусто\n"; continue; }
            int x; cout << " значение: "; cin >> x;
            cout << " узлов со значением " << x << ": " << t.countValue(x) << "\n";
        } else if (c == 5) {
            if (t.empty()) { cout << " дерево пусто\n"; continue; }
            cout << " полное: " << (t.isFull() ? "да" : "нет")
                 << ", совершенное: " << (t.isPerfect() ? "да" : "нет")
                 << ", завершённое: " << (t.isComplete() ? "да" : "нет") << "\n";
        } else if (c == 6) {
            if (t.empty()) { cout << " дерево пусто\n"; continue; }
            cout << " максимум: " << t.findMax() << "\n";
        } else if (c == 7) {
            demo();
        }
    }
    return 0;
}
