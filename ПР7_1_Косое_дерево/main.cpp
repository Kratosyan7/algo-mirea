// ============================================================================
//  Практическая работа 7_1. Деревья поиска и их применение для поиска данных.
//  Вариант 14: сбалансированное дерево - косое (splay tree);
//  запись файла - Студент (номер зачётной книжки, номер группы, ФИО).
//
//  Задание 1. Поиск записи в бинарном файле посредством БДП.
//  Задание 2. То же посредством косого дерева.
//  Задание 3. Сравнительный анализ: хеш-таблица, БДП, косое дерево.
//
//  Сборка: g++ -std=c++17 -O2 -Wall main.cpp -o trees
//  Запуск: ./trees 1 | 2 | 3   - задания, ./trees без аргументов - меню
// ============================================================================

#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstdio>
#include "Student.h"
#include "BST.h"
#include "Splay.h"
#include "Hash.h"

using namespace std;
using namespace chrono;

const string FILE10 = "students10.dat";

// десять ключей для демонстрации операций на деревьях
vector<ll> demoKeys() {
    return {50, 30, 70, 20, 40, 60, 80, 35, 45, 65};
}

vector<pair<ll,ll>> demoPairs() {
    vector<pair<ll,ll>> res;
    auto k = demoKeys();
    for (ll i = 0; i < (ll)k.size(); i++) res.push_back({k[(size_t)i], i});
    return res;
}

// Поиск записи в файле по ключу через дерево: сначала ключ ищется в дереве и
// возвращается номер записи, затем выполняется прямой доступ к файлу.
template <class Tree>
ll findRecord(Tree &t, const string &file, ll key, Student &s) {
    ll ref = t.find(key);
    if (ref < 0) return -1;
    return readRecord(file, ref, s) ? ref : -1;
}

// ===========================================================================
//  ЗАДАНИЕ 1. Бинарное дерево поиска
// ===========================================================================
void task1() {
    cout << "\n=== ЗАДАНИЕ 1. Бинарное дерево поиска ===\n";
    cout << "\n--- БДП для 10 ключей: 50 30 70 20 40 60 80 35 45 65 ---\n";
    BST t;
    t.build(demoPairs());
    t.show();
    cout << "  узлов: " << t.size() << ", высота: " << t.height() << "\n";

    cout << "\n--- Вставка ключа 55 ---\n";
    t.insert(55, 10);
    t.show();

    cout << "\n--- Удаление листа (ключ 20) ---\n";
    t.erase(20);
    t.show();

    cout << "\n--- Удаление узла с одним потомком (ключ 30: остался только "
         << "правый потомок 40) ---\n";
    t.erase(30);
    t.show();

    cout << "\n--- Удаление узла с двумя потомками (ключ 70: потомки 60 и 80, "
         << "замена минимумом правого поддерева) ---\n";
    t.erase(70);
    t.show();

    cout << "\n--- Поиск ключей ---\n";
    for (ll k : {40LL, 70LL, 99LL}) {
        cmpCount = 0;
        ll ref = t.find(k);
        cout << "  ключ " << k << ": "
             << (ref >= 0 ? "найден, запись " + to_string(ref) : string("не найден"))
             << ", сравнений " << cmpCount << "\n";
    }

    // --- работа с двоичным файлом ---
    cout << "\n--- Поиск записи в двоичном файле через БДП ---\n";
    auto special = generateFile(FILE10, 10);
    auto pairs = readKeys(FILE10);
    BST ft;
    ft.build(pairs);
    cout << "  файл из " << recordCount(FILE10) << " записей, дерево: "
         << ft.size() << " узлов, высота " << ft.height() << "\n";
    ft.show();

    Student s;
    cout << "\n  успешный поиск (ключ последней записи " << special[2] << "):\n";
    cmpCount = 0;
    ll no = findRecord(ft, FILE10, special[2], s);
    cout << "  запись " << no << ", сравнений " << cmpCount << "\n";
    printStudent(s);

    cout << "\n  безуспешный поиск (ключ 12345678):\n";
    cmpCount = 0;
    no = findRecord(ft, FILE10, 12345678, s);
    cout << "  результат: " << (no < 0 ? "запись не найдена" : "найдена")
         << ", сравнений " << cmpCount << "\n";

    // --- замеры по формату табл. 14 ---
    cout << "\n--- Тестирование на файлах 100, 1000, 10000 записей ---\n";
    printf("%8s %7s %9s %8s %9s %8s %9s %8s\n", "n", "высота",
           "T первой", "Тф перв", "T средней", "Тф сред", "T последней", "Тф посл");
    for (ll n : {100LL, 1000LL, 10000LL}) {
        string f = "bst_" + to_string(n) + ".dat";
        auto sp = generateFile(f, n);
        auto kv = readKeys(f);
        BST tree;
        tree.build(kv);

        double tm[3]; ll cf[3];
        for (int i = 0; i < 3; i++) {
            cmpCount = 0;
            findRecord(tree, f, sp[i], s);
            cf[i] = cmpCount;
            const int REP = 20000;
            ifstream in(f, ios::binary);
            auto t0 = steady_clock::now();
            for (int j = 0; j < REP; j++) {
                ll ref = tree.find(sp[i]);
                readRecord(in, ref, s);
            }
            auto t1 = steady_clock::now();
            tm[i] = duration<double, milli>(t1 - t0).count() / REP;
        }
        printf("%8lld %7d %9.5f %8lld %9.5f %8lld %11.5f %8lld\n",
               n, tree.height(), tm[0], cf[0], tm[1], cf[1], tm[2], cf[2]);
        remove(f.c_str());
    }
}

// ===========================================================================
//  ЗАДАНИЕ 2. Косое дерево
// ===========================================================================
void task2() {
    cout << "\n=== ЗАДАНИЕ 2. Косое дерево (splay tree) ===\n";
    cout << "\n--- Построение дерева из 10 ключей: 50 30 70 20 40 60 80 35 45 65 ---\n";
    SplayTree t;
    auto pairs = demoPairs();
    for (auto &kv : pairs) {
        t.insert(kv.first, kv.second);
        cout << "  после вставки " << kv.first << " корень = " << t.rootKey() << "\n";
    }
    cout << "\n  структура дерева:\n";
    t.show();
    cout << "  узлов: " << t.size() << ", высота: " << t.height() << "\n";

    cout << "\n--- Поиск ключа 20: узел поднимается в корень ---\n";
    cmpCount = 0;
    t.find(20);
    cout << "  сравнений " << cmpCount << ", корень = " << t.rootKey() << "\n";
    t.show();

    cout << "\n--- Повторный поиск того же ключа 20 ---\n";
    cmpCount = 0;
    t.find(20);
    cout << "  сравнений " << cmpCount << " (ключ уже в корне)\n";

    cout << "\n--- Поиск отсутствующего ключа 55 ---\n";
    cmpCount = 0;
    ll ref = t.find(55);
    cout << "  результат " << ref << " (не найден), сравнений " << cmpCount
         << ", корень = " << t.rootKey() << "\n";

    cout << "\n--- Вставка ключа 55 ---\n";
    t.insert(55, 10);
    t.show();

    cout << "\n--- Удаление ключа 50 ---\n";
    t.erase(50);
    t.show();

    // --- работа с двоичным файлом ---
    cout << "\n--- Поиск записи в двоичном файле через косое дерево ---\n";
    auto special = generateFile(FILE10, 10);
    auto kv = readKeys(FILE10);
    SplayTree ft;
    ft.build(kv);
    cout << "  файл из " << recordCount(FILE10) << " записей, дерево: "
         << ft.size() << " узлов, высота " << ft.height() << "\n";
    ft.show();

    Student s;
    cout << "\n  успешный поиск (ключ последней записи " << special[2] << "):\n";
    cmpCount = 0;
    ll no = findRecord(ft, FILE10, special[2], s);
    cout << "  запись " << no << ", сравнений " << cmpCount << "\n";
    printStudent(s);

    cout << "\n  безуспешный поиск (ключ 12345678): ";
    cmpCount = 0;
    no = findRecord(ft, FILE10, 12345678, s);
    cout << (no < 0 ? "запись не найдена" : "найдена")
         << ", сравнений " << cmpCount << "\n";

    cout << "\n--- Тестирование на файлах 100, 1000, 10000 записей ---\n";
    printf("%8s %7s %9s %8s %9s %8s %11s %8s %10s\n", "n", "высота",
           "T первой", "Тф перв", "T средней", "Тф сред", "T последней",
           "Тф посл", "Тф повтор");
    for (ll n : {100LL, 1000LL, 10000LL}) {
        string f = "splay_" + to_string(n) + ".dat";
        auto sp = generateFile(f, n);
        auto keys = readKeys(f);

        double tm[3]; ll cf[3];
        ll again = 0;
        for (int i = 0; i < 3; i++) {
            SplayTree tree;                 // дерево строится заново, чтобы
            tree.build(keys);               // измерить первый поиск ключа
            cmpCount = 0;
            findRecord(tree, f, sp[i], s);
            cf[i] = cmpCount;
            if (i == 2) { cmpCount = 0; tree.find(sp[i]); again = cmpCount; }
            const int REP = 20000;
            ifstream in(f, ios::binary);
            auto t0 = steady_clock::now();
            for (int j = 0; j < REP; j++) {
                ll ref = tree.find(sp[i]);
                readRecord(in, ref, s);
            }
            auto t1 = steady_clock::now();
            tm[i] = duration<double, milli>(t1 - t0).count() / REP;
        }
        SplayTree tree; tree.build(keys);
        printf("%8lld %7d %9.5f %8lld %9.5f %8lld %11.5f %8lld %10lld\n",
               n, tree.height(), tm[0], cf[0], tm[1], cf[1], tm[2], cf[2], again);
        remove(f.c_str());
    }
}

// ===========================================================================
//  ЗАДАНИЕ 3. Сравнительный анализ поисковых структур
// ===========================================================================
void task3() {
    cout << "\n=== ЗАДАНИЕ 3. Сравнение поисковых структур ===\n";
    cout << "  Ищется ключ последней записи файла.\n\n";
    printf("%-16s %9s %11s %13s %10s %13s\n", "структура", "элементов",
           "сравнений", "среднее Тф", "высота/m", "T поиска, мкс");

    for (ll n : {100LL, 1000LL, 10000LL}) {
        string f = "cmp_" + to_string(n) + ".dat";
        auto sp = generateFile(f, n);
        auto kv = readKeys(f);
        ll key = sp[2];
        Student s;

        HashTable h;   h.build(kv);
        BST bst;       bst.build(kv);
        SplayTree spl; spl.build(kv);

        auto timeIt = [&](auto &&fn) {
            const int REP = 100000;
            auto t0 = steady_clock::now();
            for (int i = 0; i < REP; i++) fn();
            auto t1 = steady_clock::now();
            return duration<double, micro>(t1 - t0).count() / REP;
        };

        // число сравнений при поиске ключа последней записи в свежей структуре
        cmpCount = 0; h.find(key);   ll ch = cmpCount;
        cmpCount = 0; bst.find(key); ll cb = cmpCount;
        SplayTree fresh; fresh.build(kv);
        cmpCount = 0; fresh.find(key); ll cs = cmpCount;

        // среднее число сравнений по всем ключам структуры
        auto avg = [&](auto &&fn) {
            cmpCount = 0;
            for (auto &p : kv) fn(p.first);
            return (double)cmpCount / kv.size();
        };
        double ah = avg([&](ll k){ h.find(k); });
        double ab = avg([&](ll k){ bst.find(k); });
        SplayTree fresh2; fresh2.build(kv);
        double as = avg([&](ll k){ fresh2.find(k); });

        double th = timeIt([&]{ h.find(key); });
        double tb = timeIt([&]{ bst.find(key); });
        double ts = timeIt([&]{ spl.find(key); });

        cmpCount = 0; linearSearch(f, key, s); ll cl = cmpCount;

        printf("%-16s %9lld %11lld %13.2f %10lld %13.4f\n",
               "Хеш-таблица", n, ch, ah, h.slots(), th);
        printf("%-16s %9lld %11lld %13.2f %10d %13.4f\n",
               "БДП", n, cb, ab, bst.height(), tb);
        printf("%-16s %9lld %11lld %13.2f %10d %13.4f\n",
               "Косое дерево", n, cs, as, spl.height(), ts);
        printf("%-16s %9lld %11lld %13s %10s %13s\n",
               "Линейный поиск", n, cl, "-", "-", "-");
        cout << "\n";
        remove(f.c_str());
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
             << " Практическая работа 7_1. Деревья поиска. Вариант 14.\n"
             << " 1 - задание 1: бинарное дерево поиска\n"
             << " 2 - задание 2: косое дерево\n"
             << " 3 - задание 3: сравнение структур\n"
             << " 0 - выход\n выбор: ";
        int c;
        if (!(cin >> c) || c == 0) break;
        if (c == 1) task1();
        else if (c == 2) task2();
        else if (c == 3) task3();
    }
    return 0;
}
