// ============================================================================
//  Hash.h - хеш-таблица с цепочками (для сравнительного анализа, задание 3).
//  Используется реализация из практической работы 6_1, адаптированная под
//  ключ типа long long.
// ============================================================================
#ifndef HASH_H
#define HASH_H

#include "Student.h"

const double MAX_LOAD = 0.75;

struct HItem { ll key, ref; };

class HashTable {
    vector<vector<HItem>> tab;
    ll cnt;

    static bool isPrime(ll n) {
        if (n < 2) return false;
        for (ll d = 2; d * d <= n; d++) if (n % d == 0) return false;
        return true;
    }
    static ll nextPrime(ll n) { while (!isPrime(n)) n++; return n; }

    void rehash() {
        ll m = nextPrime(2 * (ll)tab.size());
        vector<HItem> all;
        for (auto &ch : tab) for (auto &it : ch) all.push_back(it);
        tab.assign((size_t)m, {});
        for (auto &it : all) tab[(size_t)(it.key % m)].push_back(it);
    }

public:
    HashTable(ll m = 11) : tab((size_t)m), cnt(0) {}

    ll size() const { return cnt; }
    ll slots() const { return (ll)tab.size(); }
    double load() const { return (double)cnt / tab.size(); }

    void insert(ll key, ll ref) {
        tab[(size_t)(key % (ll)tab.size())].push_back({key, ref});
        cnt++;
        if (load() > MAX_LOAD) rehash();
    }

    ll find(ll key) const {
        for (auto &it : tab[(size_t)(key % (ll)tab.size())]) {
            cmpCount++;
            if (it.key == key) return it.ref;
        }
        return -1;
    }

    void build(const vector<pair<ll,ll>> &keys) {
        tab.assign(11, {}); cnt = 0;
        for (auto &kv : keys) insert(kv.first, kv.second);
    }
};

#endif
