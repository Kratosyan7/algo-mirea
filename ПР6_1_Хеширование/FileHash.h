// ============================================================================
//  FileHash.h - управление доступом к записям двоичного файла посредством
//  хеш-таблицы. Подключает оба предыдущих заголовочных файла.
//
//  Таблица играет роль индекса: ключ записи -> номер записи в файле.
//  Поиск записи выполняется в два шага: поиск ключа в таблице (в среднем
//  O(1) сравнений) и прямой доступ к файлу по полученному номеру.
// ============================================================================
#ifndef FILEHASH_H
#define FILEHASH_H

#include "HeshTable.h"
#include "BinFile.h"

// ---------------------------------------------------------------------------
//  Прототипы
// ---------------------------------------------------------------------------
HeshTable buildIndex(const string &bin, int m);          // индекс по файлу
bool addRecord(const string &bin, HeshTable &h, const City &c);
bool deleteRecord(const string &bin, HeshTable &h, int key);
long long findRecord(const string &bin, HeshTable &h, int key, City &c);

// ---------------------------------------------------------------------------
//  Реализация
// ---------------------------------------------------------------------------

// Прочитать файл и вставить в таблицу пары (ключ, номер записи).
// Начальный размер m берётся заведомо малым, чтобы в ходе построения
// индекса потребовалось рехеширование.
HeshTable buildIndex(const string &bin, int m) {
    HeshTable h = newTable(m);
    ifstream in(bin, ios::binary);
    City c;
    long long no = 0;
    while (in.read((char*)&c, sizeof(City))) {
        insertKey(h, c.code, (int)no);
        no++;
    }
    return h;
}

// Добавить запись в конец файла и ключ в таблицу.
bool addRecord(const string &bin, HeshTable &h, const City &c) {
    if (findKey(h, c.code) >= 0) return false;      // ключ уже есть
    long long no = recordCount(bin);
    if (!appendRecord(bin, c)) return false;
    return insertKey(h, c.code, (int)no);
}

// Удалить запись с заданным ключом из файла и из таблицы.
// Чтобы не сдвигать номера остальных записей, на место удаляемой
// переносится последняя запись файла, а её номер в таблице исправляется.
bool deleteRecord(const string &bin, HeshTable &h, int key) {
    int no = findKey(h, key);
    if (no < 0) return false;

    long long last = recordCount(bin) - 1;
    City lastRec;
    readRecord(bin, last, lastRec);

    if (no != last) {
        writeRecord(bin, no, lastRec);              // перенос последней записи
        eraseKey(h, lastRec.code);
        insertKey(h, lastRec.code, no);             // правка её номера
    }
    truncateLast(bin);
    eraseKey(h, key);
    return true;
}

// Поиск записи с заданным ключом: ключ ищется в хеш-таблице, затем
// выполняется прямой доступ к файлу по номеру записи.
long long findRecord(const string &bin, HeshTable &h, int key, City &c) {
    int no = findKey(h, key);
    if (no < 0) return -1;
    if (!readRecord(bin, no, c)) return -1;
    return no;
}

#endif
