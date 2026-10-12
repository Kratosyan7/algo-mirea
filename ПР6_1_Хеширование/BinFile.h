// ============================================================================
//  BinFile.h - операции управления двоичным файлом записей City.
//  Записи имеют постоянную длину, поэтому возможен прямой доступ:
//  запись с номером no лежит по смещению no * sizeof(City).
// ============================================================================
#ifndef BINFILE_H
#define BINFILE_H

#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <string>
#include "City.h"

using namespace std;

// ---------------------------------------------------------------------------
//  Прототипы
// ---------------------------------------------------------------------------
long long recordCount(const string &bin);                  // число записей
bool createBinFromText(const string &txt, const string &bin);  // txt -> bin
bool appendRecord(const string &bin, const City &c);       // добавить запись
bool readRecord(const string &bin, long long no, City &c);  // прочитать по номеру
bool writeRecord(const string &bin, long long no, const City &c);
bool truncateLast(const string &bin);                      // отбросить последнюю
long long linearSearchFile(const string &bin, int key, City &c); // поиск перебором
void generateFile(const string &bin, long long n);          // сгенерировать файл

extern long long cmpCount;   // счётчик сравнений определён в HeshTable.h

// ---------------------------------------------------------------------------
//  Реализация
// ---------------------------------------------------------------------------

long long recordCount(const string &bin) {
    ifstream in(bin, ios::binary | ios::ate);
    if (!in) return 0;
    return (long long)in.tellg() / sizeof(City);
}

// Создание двоичного файла из текстового. Формат строки текстового файла:
// код;название города;страна
bool createBinFromText(const string &txt, const string &bin) {
    ifstream in(txt);
    if (!in) return false;
    ofstream out(bin, ios::binary | ios::trunc);

    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        size_t p1 = line.find(';'), p2 = line.find(';', p1 + 1);
        if (p1 == string::npos || p2 == string::npos) continue;
        City c = makeCity(stoi(line.substr(0, p1)),
                          line.substr(p1 + 1, p2 - p1 - 1),
                          line.substr(p2 + 1));
        out.write((char*)&c, sizeof(City));
    }
    return true;
}

bool appendRecord(const string &bin, const City &c) {
    ofstream out(bin, ios::binary | ios::app);
    if (!out) return false;
    out.write((char*)&c, sizeof(City));
    return true;
}

// Прямой доступ: позиционирование по номеру записи и чтение
bool readRecord(const string &bin, long long no, City &c) {
    ifstream in(bin, ios::binary);
    if (!in) return false;
    in.seekg(no * (long long)sizeof(City));
    return (bool)in.read((char*)&c, sizeof(City));
}

bool writeRecord(const string &bin, long long no, const City &c) {
    fstream f(bin, ios::binary | ios::in | ios::out);
    if (!f) return false;
    f.seekp(no * (long long)sizeof(City));
    f.write((char*)&c, sizeof(City));
    return true;
}

// Отбросить последнюю запись файла (используется при удалении)
bool truncateLast(const string &bin) {
    long long n = recordCount(bin);
    if (n == 0) return false;
    vector<City> all((size_t)n - 1);
    ifstream in(bin, ios::binary);
    if (n > 1) in.read((char*)all.data(), (n - 1) * sizeof(City));
    in.close();
    ofstream out(bin, ios::binary | ios::trunc);
    if (n > 1) out.write((char*)all.data(), (n - 1) * sizeof(City));
    return true;
}

// Линейный поиск записи в файле (для сравнения с поиском через хеш-таблицу)
long long linearSearchFile(const string &bin, int key, City &c) {
    ifstream in(bin, ios::binary);
    if (!in) return -1;
    City buf;
    long long no = 0;
    while (in.read((char*)&buf, sizeof(City))) {
        cmpCount++;
        if (buf.code == key) { c = buf; return no; }
        no++;
    }
    return -1;
}

// Генерация файла из n записей с уникальными кодами городов.
// Коды подобраны так, что среди них есть дающие одинаковый хеш-код
// (коллизии), поскольку разность соседних значений кратна размеру таблицы.
void generateFile(const string &bin, long long n) {
    mt19937 rnd(20260930);
    uniform_int_distribution<int> dist(1000, 999999);
    vector<int> codes;
    codes.reserve((size_t)n);
    while ((long long)codes.size() < n) {                 // уникальные коды
        for (long long i = codes.size(); i < 2 * n; i++) codes.push_back(dist(rnd));
        sort(codes.begin(), codes.end());
        codes.erase(unique(codes.begin(), codes.end()), codes.end());
    }
    codes.resize((size_t)n);
    shuffle(codes.begin(), codes.end(), rnd);

    const vector<string> NAMES = {"Alfa", "Beta", "Gamma", "Delta", "Omega",
                                  "Sigma", "Lambda", "Kappa", "Zeta", "Theta"};
    const vector<string> LANDS = {"Rossiya", "Belarus", "Armeniya", "Kazahstan",
                                  "Uzbekistan", "Kyrgyzstan"};

    ofstream out(bin, ios::binary | ios::trunc);
    vector<City> buf;
    buf.reserve(4096);
    for (long long i = 0; i < n; i++) {
        City c = makeCity(codes[(size_t)i],
                          NAMES[rnd() % NAMES.size()] + "-" + to_string(i),
                          LANDS[rnd() % LANDS.size()]);
        buf.push_back(c);
        if (buf.size() == 4096) {
            out.write((char*)buf.data(), buf.size() * sizeof(City));
            buf.clear();
        }
    }
    if (!buf.empty()) out.write((char*)buf.data(), buf.size() * sizeof(City));
}

#endif
