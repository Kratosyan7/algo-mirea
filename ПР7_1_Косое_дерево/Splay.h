// ============================================================================
//  Splay.h - косое дерево поиска (splay tree), вариант 14, задание 2.
//
//  Косое дерево не хранит ни балансировочных множителей, ни цветов узлов.
//  Вместо этого после каждого обращения к ключу выполняется операция splay:
//  найденный (или последний просмотренный) узел поднимается в корень серией
//  вращений. Три случая подъёма: zig (узел является потомком корня),
//  zig-zig (узел и его родитель - потомки одной стороны), zig-zag (узел и
//  родитель - потомки разных сторон). Амортизированная сложность любой
//  операции составляет O(log n), а повторное обращение к тому же ключу
//  выполняется за одно сравнение, так как ключ уже находится в корне.
// ============================================================================
#ifndef SPLAY_H
#define SPLAY_H

#include "Student.h"

struct SNode {
    ll key, ref;
    SNode *left, *right;
    SNode(ll k, ll r) : key(k), ref(r), left(nullptr), right(nullptr) {}
};

class SplayTree {
    SNode *root;
    ll cnt;

    void clear(SNode *p) {
        if (!p) return;
        clear(p->left); clear(p->right);
        delete p;
    }

    // правое вращение вокруг узла p (левый потомок поднимается)
    SNode* rotateRight(SNode *p) {
        SNode *q = p->left;
        p->left = q->right;
        q->right = p;
        return q;
    }

    // левое вращение вокруг узла p (правый потомок поднимается)
    SNode* rotateLeft(SNode *p) {
        SNode *q = p->right;
        p->right = q->left;
        q->left = p;
        return q;
    }

    // Операция splay: поднимает узел с ключом key в корень поддерева p.
    // Если ключа нет, в корень поднимается последний просмотренный узел.
    SNode* splay(SNode *p, ll key) {
        if (!p) return nullptr;
        cmpCount++;
        if (p->key == key) return p;

        if (key < p->key) {                      // идём влево
            if (!p->left) return p;
            cmpCount++;
            if (key < p->left->key) {            // zig-zig (левый-левый)
                p->left->left = splay(p->left->left, key);
                p = rotateRight(p);
            } else if (key > p->left->key) {     // zig-zag (левый-правый)
                p->left->right = splay(p->left->right, key);
                if (p->left->right) p->left = rotateLeft(p->left);
            }
            return p->left ? rotateRight(p) : p;  // zig
        } else {                                 // идём вправо
            if (!p->right) return p;
            cmpCount++;
            if (key > p->right->key) {           // zig-zig (правый-правый)
                p->right->right = splay(p->right->right, key);
                p = rotateLeft(p);
            } else if (key < p->right->key) {    // zig-zag (правый-левый)
                p->right->left = splay(p->right->left, key);
                if (p->right->left) p->right = rotateRight(p->right);
            }
            return p->right ? rotateLeft(p) : p;  // zig
        }
    }

    void show(SNode *p, int level) const {
        if (!p) return;
        show(p->right, level + 1);
        cout << string(level * 4, ' ') << p->key << " (зап. " << p->ref << ")\n";
        show(p->left, level + 1);
    }

    int height(SNode *p) const {
        return p ? 1 + max(height(p->left), height(p->right)) : 0;
    }

public:
    SplayTree() : root(nullptr), cnt(0) {}
    ~SplayTree() { clear(root); }

    ll size() const { return cnt; }
    int height() const { return height(root); }
    bool empty() const { return !root; }
    ll rootKey() const { return root ? root->key : -1; }

    // Включение элемента: дерево разрезается по ключу, новый узел становится
    // корнем, а части дерева подвешиваются к нему.
    bool insert(ll key, ll ref) {
        if (!root) { root = new SNode(key, ref); cnt++; return true; }
        root = splay(root, key);
        if (root->key == key) return false;       // ключ уже есть

        SNode *p = new SNode(key, ref);
        if (key < root->key) {
            p->right = root;
            p->left = root->left;
            root->left = nullptr;
        } else {
            p->left = root;
            p->right = root->right;
            root->right = nullptr;
        }
        root = p;
        cnt++;
        return true;
    }

    // Поиск ключа с возвратом ссылки. После поиска найденный узел
    // оказывается в корне, поэтому повторный поиск того же ключа выполняется
    // за одно сравнение.
    ll find(ll key) {
        if (!root) return -1;
        root = splay(root, key);
        return root->key == key ? root->ref : -1;
    }

    // Удаление ключа: узел поднимается в корень и удаляется, после чего
    // максимальный ключ левого поддерева поднимается в его корень и к нему
    // подвешивается правое поддерево.
    bool erase(ll key) {
        if (!root) return false;
        root = splay(root, key);
        if (root->key != key) return false;

        SNode *old = root;
        if (!root->left) root = root->right;
        else {
            SNode *rightPart = root->right;
            root = splay(root->left, key);        // в корне окажется максимум
            root->right = rightPart;
        }
        delete old;
        cnt--;
        return true;
    }

    void show() const {
        if (!root) { cout << "  дерево пусто\n"; return; }
        show(root, 0);
    }

    void build(const vector<pair<ll,ll>> &keys) {
        clear(root); root = nullptr; cnt = 0;
        for (auto &kv : keys) insert(kv.first, kv.second);
    }
};

#endif
