// ============================================================================
//  City.h - структура записи, определённая вариантом 14.
//  Справочная межгорода: код города (КЛЮЧ), название города, страна.
// ============================================================================
#ifndef CITY_H
#define CITY_H

#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>

using namespace std;

const int NAME_LEN    = 32;   // название города
const int COUNTRY_LEN = 24;   // страна

struct City {
    int  code;                 // КЛЮЧ: междугородный телефонный код города
    char name[NAME_LEN];       // название города
    char country[COUNTRY_LEN]; // страна
};

// записать строку в символьное поле фиксированной длины
inline void putStr(char *dst, const string &src, int len) {
    fill(dst, dst + len, '\0');
    copy_n(src.begin(), min((int)src.size(), len - 1), dst);
}

// собрать запись из отдельных полей
inline City makeCity(int code, const string &name, const string &country) {
    City c;
    c.code = code;
    putStr(c.name, name, NAME_LEN);
    putStr(c.country, country, COUNTRY_LEN);
    return c;
}

inline void printCity(const City &c) {
    cout << setw(6) << c.code << "  " << setw(20) << left << c.name
         << "  " << c.country << right << "\n";
}

inline void printCityHeader() {
    cout << "  код  " << setw(20) << left << "город" << "  страна" << right << "\n";
    cout << "  ------------------------------------------------\n";
}

#endif
