// ============================================================================
//  HeshTable.h - хеш-таблица с разрешением коллизий методом цепочек
//                (цепное хеширование, вариант 14).
//
//  Элемент таблицы хранит: ключ записи, номер записи в исходной структуре
//  (массиве или файле) и признак prK - хеш-код, по которому ключ включён.
//  Для метода цепочек prK всегда равен h(key), поле сохранено, поскольку
//  требуется структурой элемента и используется при выводе таблицы.
// ============================================================================
#ifndef HESHTABLE_H
#define HESHTABLE_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "City.h"

using namespace std;

const int    START_SIZE = 11;     // начальный размер таблицы (простое число)
const double MAX_LOAD   = 0.75;   // коэффициент нагрузки, выше - рехеширование

// счётчик сравнений ключей, обнуляется перед каждым замером
long long cmpCount = 0;
// печатать ли сообщения о рехешировании (отключается при массовых замерах)
bool verboseRehash = true;

struct Item {          // элемент хеш-таблицы
    int key;           // ключ записи
    int idx;           // номер записи в исходной структуре
    int prK;           // хеш-код, по которому ключ включён в таблицу
};

struct HeshTable {
    vector<vector<Item>> tab;   // tab[i] - цепочка (список) i-го слота
    int size;                   // размер таблицы (количество слотов)
    int count;                  // количество ключей в таблице
};

// ---------------------------------------------------------------------------
//  Прототипы операций управления хеш-таблицей
// ---------------------------------------------------------------------------
HeshTable newTable(int m);                          // создать пустую таблицу
int    hashCode(int key, int m);                    // хеш-функция
bool   insertKey(HeshTable &h, int key, int idx);   // вставить ключ
bool   eraseKey(HeshTable &h, int key);             // удалить ключ
int    findKey(HeshTable &h, int key);              // найти ключ, вернуть idx
void   printTable(const HeshTable &h);              // вывести таблицу
void   rehashTable(HeshTable &h);                   // рехешировать таблицу
HeshTable createTable(const vector<City> &arr);     // построить по массиву
double loadFactor(const HeshTable &h);              // коэффициент нагрузки
int    nextPrime(int n);                            // следующее простое число
void   testHeshT();                                 // тестирование операций

// ---------------------------------------------------------------------------
//  Реализация
// ---------------------------------------------------------------------------

// простое ли число
bool isPrime(int n) {
    if (n < 2) return false;
    for (int d = 2; d * d <= n; d++)
        if (n % d == 0) return false;
    return true;
}

// наименьшее простое число, не меньшее n
int nextPrime(int n) {
    while (!isPrime(n)) n++;
    return n;
}

HeshTable newTable(int m) {
    HeshTable h;
    h.size = m;
    h.count = 0;
    h.tab.assign(m, {});
    return h;
}

// Хеш-функция: метод деления. Размер таблицы - простое число, поэтому
// остаток от деления равномерно рассеивает коды городов по слотам.
int hashCode(int key, int m) {
    return (key % m + m) % m;
}

double loadFactor(const HeshTable &h) {
    return (double)h.count / h.size;
}

// Вставка ключа. Возвращает false, если ключ уже есть в таблице.
// После вставки проверяется коэффициент нагрузки и при необходимости
// выполняется рехеширование.
bool insertKey(HeshTable &h, int key, int idx) {
    int i = hashCode(key, h.size);
    for (auto &it : h.tab[i]) {
        cmpCount++;
        if (it.key == key) return false;       // дубликат ключа
    }
    h.tab[i].push_back({key, idx, i});
    h.count++;
    if (loadFactor(h) > MAX_LOAD) rehashTable(h);
    return true;
}

// Удаление ключа из таблицы.
bool eraseKey(HeshTable &h, int key) {
    int i = hashCode(key, h.size);
    auto &chain = h.tab[i];
    for (size_t k = 0; k < chain.size(); k++) {
        cmpCount++;
        if (chain[k].key == key) {
            chain.erase(chain.begin() + k);
            h.count--;
            return true;
        }
    }
    return false;
}

// Поиск ключа. Возвращает номер записи в исходной структуре или -1.
// В cmpCount накапливается число сравнений ключей.
int findKey(HeshTable &h, int key) {
    int i = hashCode(key, h.size);
    for (auto &it : h.tab[i]) {
        cmpCount++;
        if (it.key == key) return it.idx;
    }
    return -1;
}

// Рехеширование: новый размер - простое число, не меньшее удвоенного
// текущего; все ключи переносятся в новую таблицу заново.
void rehashTable(HeshTable &h) {
    int m = nextPrime(2 * h.size);
    if (verboseRehash)
        cout << "  [рехеширование] alpha = " << fixed << setprecision(3)
             << loadFactor(h) << " > " << MAX_LOAD
             << ", размер таблицы " << h.size << " -> " << m << "\n";

    vector<Item> all;
    for (auto &chain : h.tab)
        for (auto &it : chain) all.push_back(it);

    h = newTable(m);
    for (auto &it : all) {
        int i = hashCode(it.key, m);
        h.tab[i].push_back({it.key, it.idx, i});
        h.count++;
    }
}

// Вывод таблицы: для каждого слота печатается его цепочка.
void printTable(const HeshTable &h) {
    cout << "  размер таблицы m = " << h.size << ", ключей n = " << h.count
         << ", alpha = " << fixed << setprecision(3) << loadFactor(h) << "\n";
    for (int i = 0; i < h.size; i++) {
        cout << "  [" << setw(2) << i << "] ";
        if (h.tab[i].empty()) cout << "-";
        for (size_t k = 0; k < h.tab[i].size(); k++) {
            if (k) cout << " -> ";
            cout << "(key=" << h.tab[i][k].key
                 << ", idx=" << h.tab[i][k].idx
                 << ", prK=" << h.tab[i][k].prK << ")";
        }
        cout << "\n";
    }
}

// Создание хеш-таблицы по данным исходного массива записей.
HeshTable createTable(const vector<City> &arr) {
    HeshTable h = newTable(START_SIZE);
    for (int i = 0; i < (int)arr.size(); i++) insertKey(h, arr[i].code, i);
    return h;
}

// Среднее число сравнений при успешном поиске по всем ключам таблицы
double avgProbes(HeshTable &h) {
    vector<int> keys;
    for (auto &chain : h.tab)
        for (auto &it : chain) keys.push_back(it.key);
    if (keys.empty()) return 0;
    long long save = cmpCount;
    cmpCount = 0;
    for (int k : keys) findKey(h, k);
    double res = (double)cmpCount / keys.size();
    cmpCount = save;
    return res;
}

// Длина самой длинной цепочки таблицы
int maxChain(const HeshTable &h) {
    size_t mx = 0;
    for (auto &chain : h.tab) mx = max(mx, chain.size());
    return (int)mx;
}

// ---------------------------------------------------------------------------
//  Тестовый (исходный) массив из 20 записей варианта 14
// ---------------------------------------------------------------------------
vector<City> testArray() {
    return {
        makeCity(495, "Moskva",        "Rossiya"),
        makeCity(812, "Sankt-Peterburg","Rossiya"),
        makeCity(383, "Novosibirsk",   "Rossiya"),
        makeCity(343, "Ekaterinburg",  "Rossiya"),
        makeCity(843, "Kazan",         "Rossiya"),
        makeCity(831, "Nizhniy Novgorod","Rossiya"),
        makeCity(351, "Chelyabinsk",   "Rossiya"),
        makeCity(846, "Samara",        "Rossiya"),
        makeCity(863, "Rostov-na-Donu","Rossiya"),
        makeCity(347, "Ufa",           "Rossiya"),
        makeCity(913, "Krasnoyarsk",   "Rossiya"),
        makeCity(862, "Sochi",         "Rossiya"),
        makeCity(423, "Vladivostok",   "Rossiya"),
        makeCity(375, "Minsk",         "Belarus"),
        makeCity(374, "Erevan",        "Armeniya"),
        makeCity(994, "Baku",          "Azerbaydzhan"),
        makeCity(996, "Bishkek",       "Kyrgyzstan"),
        makeCity(998, "Tashkent",      "Uzbekistan"),
        makeCity(371, "Riga",          "Latviya"),
        makeCity(372, "Tallin",        "Estoniya")
    };
}

// ---------------------------------------------------------------------------
//  Тестирование операций управления хеш-таблицей (задание 1)
// ---------------------------------------------------------------------------
void testHeshT() {
    vector<City> arr = testArray();

    cout << "\n=== Исходный массив (20 записей) ===\n";
    printCityHeader();
    for (int i = 0; i < (int)arr.size(); i++) {
        cout << setw(3) << i << ".";
        printCity(arr[i]);
    }

    cout << "\n=== Хеш-коды ключей при m = " << START_SIZE << " ===\n";
    for (auto &c : arr)
        cout << "  h(" << c.code << ") = " << c.code << " % " << START_SIZE
             << " = " << hashCode(c.code, START_SIZE) << "\n";

    // --- таблица до рехеширования -----------------------------------------
    // 8 ключей при m = 11 дают alpha = 0.727 < 0.75, рехеширование не нужно
    cout << "\n=== Таблица до рехеширования (8 ключей) ===\n";
    HeshTable h = newTable(START_SIZE);
    for (int i = 0; i < 8; i++) insertKey(h, arr[i].code, i);
    printTable(h);

    // --- вставка, вызывающая рехеширование ---------------------------------
    cout << "\n=== Вставка 9-го ключа (" << arr[8].code
         << "): alpha станет 9/11 = 0.818 > 0.75 ===\n";
    insertKey(h, arr[8].code, 8);
    printTable(h);

    // --- остальные ключи ----------------------------------------------------
    cout << "\n=== Таблица после включения всех 20 ключей ===\n";
    for (int i = 9; i < (int)arr.size(); i++) insertKey(h, arr[i].code, i);
    printTable(h);

    // --- вставка без коллизии и с коллизией ---------------------------------
    cout << "\n=== Вставка нового ключа без коллизии ===\n";
    int freeSlot = -1;
    for (int i = 0; i < h.size && freeSlot < 0; i++)
        if (h.tab[i].empty()) freeSlot = i;
    int keyFree = 1000 + freeSlot;
    while (hashCode(keyFree, h.size) != freeSlot) keyFree++;
    cout << "  ключ " << keyFree << ", h = " << hashCode(keyFree, h.size)
         << " (слот был пуст)\n";
    arr.push_back(makeCity(keyFree, "Test-free", "Test"));
    insertKey(h, keyFree, (int)arr.size() - 1);

    cout << "\n=== Вставка ключа с разрешением коллизии ===\n";
    int busy = hashCode(arr[0].code, h.size);
    int keyCol = arr[0].code + h.size;          // тот же остаток от деления
    cout << "  ключ " << keyCol << ", h = " << hashCode(keyCol, h.size)
         << " совпадает с h(" << arr[0].code << ") = " << busy
         << ", ключ дописывается в конец цепочки\n";
    arr.push_back(makeCity(keyCol, "Test-collision", "Test"));
    insertKey(h, keyCol, (int)arr.size() - 1);
    printTable(h);

    // --- поиск --------------------------------------------------------------
    cout << "\n=== Поиск ключей ===\n";
    for (int key : {arr[0].code, keyCol, 100500}) {
        cmpCount = 0;
        int idx = findKey(h, key);
        cout << "  поиск " << key << ": ";
        if (idx >= 0) {
            cout << "найден, индекс в массиве " << idx << ", сравнений "
                 << cmpCount << "\n      ";
            printCity(arr[idx]);
        } else {
            cout << "не найден, сравнений " << cmpCount << "\n";
        }
    }

    // --- удаление -----------------------------------------------------------
    cout << "\n=== Удаление ключа " << keyCol << " ===\n";
    eraseKey(h, keyCol);
    printTable(h);
    cmpCount = 0;
    cout << "  повторный поиск " << keyCol << ": "
         << (findKey(h, keyCol) < 0 ? "не найден" : "найден")
         << ", сравнений " << cmpCount << "\n";
}

#endif
