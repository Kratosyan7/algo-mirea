// ============================================================================
//  Практическая работа 6_1. Применение хеш-таблицы для оптимизации поиска.
//  Вариант 14: цепное хеширование; запись - Справочная межгорода
//  (код города - ключ, название города, страна).
//
//  Сборка: g++ -std=c++17 -O2 -Wall main.cpp -o hash
//  Запуск: ./hash        - текстовое меню
//          ./hash 1      - задание 1 (тест хеш-таблицы)
//          ./hash 2      - задание 2 (операции над файлом через хеш-таблицу)
//          ./hash 3      - задание 2 (замеры на файлах 100, 1000, 10000)
// ============================================================================

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <chrono>
#include <cstdio>
#include "FileHash.h"

using namespace std;
using namespace chrono;

const string TXT = "cities.txt";       // исходный текстовый файл
const string BIN = "cities.dat";       // двоичный файл записей

// ---------------------------------------------------------------------------
//  Задание 2, часть 1: демонстрация операций над файлом через хеш-таблицу
// ---------------------------------------------------------------------------
void demoFile() {
    // текстовый файл с исходными данными
    ofstream txt(TXT);
    for (auto &c : testArray())
        txt << c.code << ";" << c.name << ";" << c.country << "\n";
    txt.close();

    cout << "\n=== Создание двоичного файла из текстового ===\n";
    createBinFromText(TXT, BIN);
    cout << "  записей в файле: " << recordCount(BIN) << "\n";

    cout << "\n=== Построение хеш-таблицы (индекса) по файлу, m = 11 ===\n";
    HeshTable h = buildIndex(BIN, START_SIZE);
    printTable(h);

    cout << "\n=== Поиск записи по ключу через хеш-таблицу ===\n";
    City c;
    printCityHeader();
    for (int key : {495, 998, 777}) {
        cmpCount = 0;
        long long no = findRecord(BIN, h, key, c);
        cout << "  ключ " << key << ": ";
        if (no >= 0) {
            cout << "запись " << no << ", сравнений " << cmpCount << "\n     ";
            printCity(c);
        } else {
            cout << "не найден, сравнений " << cmpCount << "\n";
        }
    }

    cout << "\n=== Добавление записи (код 473, Voronezh) ===\n";
    addRecord(BIN, h, makeCity(473, "Voronezh", "Rossiya"));
    cout << "  записей в файле: " << recordCount(BIN) << "\n";
    cmpCount = 0;
    long long no = findRecord(BIN, h, 473, c);
    cout << "  поиск 473: запись " << no << ", сравнений " << cmpCount << "\n     ";
    printCity(c);

    cout << "\n=== Удаление записи с ключом 843 ===\n";
    deleteRecord(BIN, h, 843);
    cout << "  записей в файле: " << recordCount(BIN) << "\n";
    cmpCount = 0;
    cout << "  поиск 843: "
         << (findRecord(BIN, h, 843, c) < 0 ? "не найден" : "найден")
         << ", сравнений " << cmpCount << "\n";
    printTable(h);
}

// ---------------------------------------------------------------------------
//  Задание 2, часть 2: тестирование на файлах 100, 1000, 10000 записей
// ---------------------------------------------------------------------------
void runTests() {
    cout << "\n=== Тестирование поиска (формат табл. 11) ===\n";
    printf("%8s %7s %7s %8s %8s %8s %8s %9s %10s %12s\n",
           "n", "m", "alpha", "первая", "средняя", "последн", "среднее",
           "макс цеп", "линейный", "T, мс");
    verboseRehash = false;

    for (long long n : {100LL, 1000LL, 10000LL}) {
        string bin = "test_" + to_string(n) + ".dat";
        generateFile(bin, n);

        HeshTable h = buildIndex(bin, START_SIZE);   // с рехешированием

        City first, mid, last, tmp;
        readRecord(bin, 0, first);
        readRecord(bin, n / 2, mid);
        readRecord(bin, n - 1, last);

        long long c1, c2, c3, cl;
        cmpCount = 0; findRecord(bin, h, first.code, tmp); c1 = cmpCount;
        cmpCount = 0; findRecord(bin, h, mid.code,   tmp); c2 = cmpCount;
        cmpCount = 0; findRecord(bin, h, last.code,  tmp); c3 = cmpCount;
        cmpCount = 0; linearSearchFile(bin, last.code, tmp); cl = cmpCount;

        // среднее время поиска записи через хеш-таблицу
        const int REP = 2000;
        auto t0 = steady_clock::now();
        for (int i = 0; i < REP; i++) findRecord(bin, h, last.code, tmp);
        auto t1 = steady_clock::now();
        double ms = duration<double, milli>(t1 - t0).count() / REP;

        printf("%8lld %7d %7.3f %8lld %8lld %8lld %8.2f %9d %10lld %12.5f\n",
               n, h.size, loadFactor(h), c1, c2, c3, avgProbes(h),
               maxChain(h), cl, ms);

        remove(bin.c_str());
    }
}

// ---------------------------------------------------------------------------
int main(int argc, char **argv) {
    ios::sync_with_stdio(0);

    if (argc > 1) {
        string a = argv[1];
        if (a == "1") testHeshT();
        else if (a == "2") demoFile();
        else if (a == "3") runTests();
        return 0;
    }

    while (true) {
        cout << "\n---------------------------------------------\n"
             << " Практическая работа 6_1. Вариант 14.\n"
             << " 1 - задание 1: операции над хеш-таблицей\n"
             << " 2 - задание 2: файл через хеш-таблицу\n"
             << " 3 - задание 2: замеры на 100, 1000, 10000 записей\n"
             << " 0 - выход\n"
             << " выбор: ";
        int k;
        if (!(cin >> k) || k == 0) break;
        if (k == 1) testHeshT();
        else if (k == 2) demoFile();
        else if (k == 3) runTests();
        else cout << " нет такого пункта\n";
    }
    return 0;
}
