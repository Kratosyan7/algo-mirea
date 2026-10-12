// ============================================================================
//  Student.h - запись двоичного файла (вариант 14) и операции над файлом.
//  Студент: номер зачётной книжки (КЛЮЧ), номер группы, ФИО.
// ============================================================================
#ifndef STUDENT_H
#define STUDENT_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

using namespace std;

typedef long long ll;

const int GROUP_LEN = 12;
const int FIO_LEN   = 52;

struct Student {
    ll   book;                 // КЛЮЧ: номер зачётной книжки
    char group[GROUP_LEN];     // номер группы
    char fio[FIO_LEN];         // ФИО
};

ll cmpCount = 0;               // счётчик сравнений ключей

inline void putStr(char *dst, const string &src, int len) {
    fill(dst, dst + len, '\0');
    copy_n(src.begin(), min((int)src.size(), len - 1), dst);
}

inline Student makeStudent(ll book, const string &group, const string &fio) {
    Student s;
    s.book = book;
    putStr(s.group, group, GROUP_LEN);
    putStr(s.fio, fio, FIO_LEN);
    return s;
}

inline void printStudent(const Student &s) {
    cout << "  " << setw(8) << s.book << "  " << setw(10) << left << s.group
         << "  " << s.fio << right << "\n";
}

// Генерация файла из n записей с уникальными случайными ключами.
// Возвращает ключи первой, средней и последней записи файла.
inline vector<ll> generateFile(const string &name, ll n) {
    mt19937_64 rnd(20260930);
    uniform_int_distribution<ll> dist(10000000, 99999999);
    vector<ll> keys;
    while ((ll)keys.size() < n) {
        for (ll i = keys.size(); i < 2 * n; i++) keys.push_back(dist(rnd));
        sort(keys.begin(), keys.end());
        keys.erase(unique(keys.begin(), keys.end()), keys.end());
    }
    keys.resize((size_t)n);
    shuffle(keys.begin(), keys.end(), rnd);

    const vector<string> FAM = {"Ivanov", "Petrov", "Sidorov", "Smirnov",
                                "Kuznetsov", "Popov", "Volkov", "Morozov"};
    const vector<string> NAM = {"Ivan", "Petr", "Anna", "Elena", "Sergey",
                                "Olga", "Dmitriy", "Mariya"};

    ofstream out(name, ios::binary | ios::trunc);
    vector<Student> buf;
    buf.reserve(4096);
    for (ll i = 0; i < n; i++) {
        buf.push_back(makeStudent(keys[(size_t)i],
            "IKBO-" + to_string(10 + rnd() % 40) + "-2" + to_string(rnd() % 6),
            FAM[rnd() % FAM.size()] + " " + NAM[rnd() % NAM.size()]));
        if (buf.size() == 4096) {
            out.write((char*)buf.data(), buf.size() * sizeof(Student));
            buf.clear();
        }
    }
    if (!buf.empty()) out.write((char*)buf.data(), buf.size() * sizeof(Student));
    return {keys.front(), keys[(size_t)n / 2], keys.back()};
}

inline ll recordCount(const string &name) {
    ifstream in(name, ios::binary | ios::ate);
    return in ? (ll)in.tellg() / (ll)sizeof(Student) : 0;
}

// Прямой доступ: чтение записи по её номеру из открытого файла
inline bool readRecord(ifstream &in, ll no, Student &s) {
    in.clear();
    in.seekg(no * (ll)sizeof(Student));
    return (bool)in.read((char*)&s, sizeof(Student));
}

// То же по имени файла (файл открывается на время чтения)
inline bool readRecord(const string &name, ll no, Student &s) {
    ifstream in(name, ios::binary);
    return in ? readRecord(in, no, s) : false;
}

// Чтение всех пар (ключ, номер записи) из файла
inline vector<pair<ll,ll>> readKeys(const string &name) {
    vector<pair<ll,ll>> res;
    ifstream in(name, ios::binary);
    Student s;
    ll no = 0;
    while (in.read((char*)&s, sizeof(Student))) res.push_back({s.book, no++});
    return res;
}

// Линейный поиск записи в файле (для сравнения)
inline ll linearSearch(const string &name, ll key, Student &s) {
    ifstream in(name, ios::binary);
    Student buf;
    ll no = 0;
    while (in.read((char*)&buf, sizeof(Student))) {
        cmpCount++;
        if (buf.book == key) { s = buf; return no; }
        no++;
    }
    return -1;
}

#endif
