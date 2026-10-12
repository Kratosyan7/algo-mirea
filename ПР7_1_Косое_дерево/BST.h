// ============================================================================
//  BST.h - бинарное дерево поиска (задание 1).
//  Информационная часть узла: ключ записи и ссылка (номер записи в файле).
// ============================================================================
#ifndef BST_H
#define BST_H

#include "Student.h"

struct BNode {
    ll key, ref;                   // ключ и номер записи в файле
    BNode *left, *right;
    BNode(ll k, ll r) : key(k), ref(r), left(nullptr), right(nullptr) {}
};

class BST {
    BNode *root;
    ll cnt;

    void clear(BNode *p) {
        if (!p) return;
        clear(p->left); clear(p->right);
        delete p;
    }

    void show(BNode *p, int level) const {
        if (!p) return;
        show(p->right, level + 1);
        cout << string(level * 4, ' ') << p->key << " (зап. " << p->ref << ")\n";
        show(p->left, level + 1);
    }

    int height(BNode *p) const {
        return p ? 1 + max(height(p->left), height(p->right)) : 0;
    }

public:
    BST() : root(nullptr), cnt(0) {}
    ~BST() { clear(root); }

    ll size() const { return cnt; }
    int height() const { return height(root); }
    bool empty() const { return !root; }

    // Включение элемента в дерево. Возвращает false, если ключ уже есть.
    bool insert(ll key, ll ref) {
        BNode **p = &root;
        while (*p) {
            cmpCount++;
            if (key == (*p)->key) return false;
            p = (key < (*p)->key) ? &(*p)->left : &(*p)->right;
        }
        *p = new BNode(key, ref);
        cnt++;
        return true;
    }

    // Поиск ключа в дереве с возвратом ссылки (номера записи) или -1
    ll find(ll key) const {
        BNode *p = root;
        while (p) {
            cmpCount++;
            if (key == p->key) return p->ref;
            p = (key < p->key) ? p->left : p->right;
        }
        return -1;
    }

    // Удаление ключа из дерева. Рассматриваются три случая: узел без
    // потомков, узел с одним потомком, узел с двумя потомками (заменяется
    // минимальным ключом правого поддерева).
    bool erase(ll key) {
        BNode **p = &root;
        while (*p) {
            cmpCount++;
            if (key == (*p)->key) break;
            p = (key < (*p)->key) ? &(*p)->left : &(*p)->right;
        }
        if (!*p) return false;

        BNode *t = *p;
        if (!t->left) *p = t->right;                 // нет левого потомка
        else if (!t->right) *p = t->left;            // нет правого потомка
        else {                                       // два потомка
            BNode **m = &t->right;
            while ((*m)->left) m = &(*m)->left;      // минимум правого поддерева
            BNode *mn = *m;
            *m = mn->right;
            mn->left = t->left;
            mn->right = t->right;
            *p = mn;
        }
        delete t;
        cnt--;
        return true;
    }

    void show() const {
        if (!root) { cout << "  дерево пусто\n"; return; }
        show(root, 0);
    }

    // Построение дерева по данным файла
    void build(const vector<pair<ll,ll>> &keys) {
        clear(root); root = nullptr; cnt = 0;
        for (auto &kv : keys) insert(kv.first, kv.second);
    }
};

#endif
