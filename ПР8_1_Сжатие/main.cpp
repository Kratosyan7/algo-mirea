// ============================================================================
//  Практическая работа 8_1. Кодирование и сжатие данных методами без потерь.
//  Вариант 14. Исходные данные:
//    задания 2 и 4 (LZ77, LZ78, текст):   porpoterpoterporter
//    задание 3 (LZ77, двоичный код):      0010100110010000001
//    задание 5 (Шеннон-Фано):  Ана-дэус-рики-паки, Дормы-кормыконсту-таки,
//                              Энус-дэус-кана-дэусБАЦ!
//    задание 6 (Хаффман):      Мирзоян Ашот Оганесович
//
//  Сборка: g++ -std=c++17 -O2 -Wall main.cpp -o pack
//  Запуск: ./pack 1..6 - задания, без аргументов - меню
// ============================================================================

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <cmath>

using namespace std;

typedef long long ll;

// ---------------------------------------------------------------------------
//  Работа с символами UTF-8: текст разбивается на символы, чтобы русские
//  буквы обрабатывались как один символ, а не как два байта.
// ---------------------------------------------------------------------------
vector<string> toChars(const string &s) {
    vector<string> res;
    for (size_t i = 0; i < s.size();) {
        unsigned char c = s[i];
        int len = (c < 0x80) ? 1 : (c < 0xE0) ? 2 : (c < 0xF0) ? 3 : 4;
        res.push_back(s.substr(i, len));
        i += len;
    }
    return res;
}

string join(const vector<string> &v) {
    string s;
    for (auto &x : v) s += x;
    return s;
}

// ===========================================================================
//  ЗАДАНИЕ 1. Кодирование длин серий (RLE)
// ===========================================================================

// Простой RLE: каждая серия кодируется парой «символ + количество».
string rleEncode(const string &s) {
    string out;
    for (size_t i = 0; i < s.size();) {
        size_t j = i;
        while (j < s.size() && s[j] == s[i] && j - i < 9) j++;
        out += s[i];
        out += char('0' + (j - i));
        i = j;
    }
    return out;
}

string rleDecode(const string &s) {
    string out;
    for (size_t i = 0; i + 1 < s.size(); i += 2)
        out.append((size_t)(s[i + 1] - '0'), s[i]);
    return out;
}

// RLE с разделением текста на блоки: серия длиной больше единицы кодируется
// парой «счётчик со знаком минус + символ», а последовательность неповторяющихся
// символов - парой «счётчик + сами символы». Такой вариант не увеличивает
// размер текста, в котором мало повторов.
string rleBlockEncode(const string &s) {
    string out;
    size_t i = 0;
    while (i < s.size()) {
        size_t j = i;
        while (j < s.size() && s[j] == s[i] && j - i < 9) j++;
        if (j - i > 1) {                       // серия
            out += '-';
            out += char('0' + (j - i));
            out += s[i];
            i = j;
        } else {                               // блок неповторяющихся символов
            size_t k = i;
            while (k < s.size() && k - i < 9 &&
                   (k + 1 >= s.size() || s[k + 1] != s[k])) k++;
            out += char('0' + (k - i));
            out += s.substr(i, k - i);
            i = k;
        }
    }
    return out;
}

string rleBlockDecode(const string &s) {
    string out;
    size_t i = 0;
    while (i < s.size()) {
        if (s[i] == '-') {
            int cnt = s[i + 1] - '0';
            out.append((size_t)cnt, s[i + 2]);
            i += 3;
        } else {
            int cnt = s[i] - '0';
            out += s.substr(i + 1, cnt);
            i += 1 + cnt;
        }
    }
    return out;
}

double kRatio(size_t src, size_t dst) { return dst ? (double)src / dst : 0; }

void task1() {
    cout << "\n=== ЗАДАНИЕ 1. Групповое кодирование RLE ===\n";

    string t1 = "AAAAAAAABBBBBBCCCCCCCCCCDDDDEEEEEEE";
    string t2 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string t3 = "ABBBBBCDEFFFFFFGHIJKKKKLMNO";

    auto show = [&](const string &title, const string &src, bool block) {
        string enc = block ? rleBlockEncode(src) : rleEncode(src);
        string dec = block ? rleBlockDecode(enc) : rleDecode(enc);
        cout << "\n  " << title << "\n"
             << "  исходный текст (" << src.size() << " байт): " << src << "\n"
             << "  сжатый текст   (" << enc.size() << " байт): " << enc << "\n"
             << "  восстановлено  (" << dec.size() << " байт): " << dec << "\n"
             << "  совпадает с исходным: " << (dec == src ? "да" : "нет") << "\n"
             << "  коэффициент сжатия K = " << fixed << setprecision(3)
             << kRatio(src.size(), enc.size()) << "\n";
    };

    show("Текст 1. Длинные серии повторяющихся символов, простой RLE:", t1, false);
    show("Текст 2. Все символы различны, простой RLE:", t2, false);
    show("Текст 3. Серии вперемешку с одиночными символами, простой RLE:", t3, false);
    cout << "\n  Простой RLE удваивает размер текста без повторов, поэтому "
         << "применяется\n  алгоритм разделения текста на блоки серий и блоки "
         << "одиночных символов:\n";
    show("Текст 2, алгоритм с разделением:", t2, true);
    show("Текст 3, алгоритм с разделением:", t3, true);

    // сжатие файла
    ofstream f("rle_in.txt");
    string big;
    for (int i = 0; i < 200; i++) {
        big += string(5 + i % 9, 'A' + i % 6);
        big += "xyz";
    }
    f << big;
    f.close();
    string enc = rleBlockEncode(big), dec = rleBlockDecode(enc);
    ofstream("rle_out.rle") << enc;
    ofstream("rle_back.txt") << dec;
    cout << "\n  Сжатие текстового файла:\n"
         << "  rle_in.txt   : " << big.size() << " байт\n"
         << "  rle_out.rle  : " << enc.size() << " байт\n"
         << "  rle_back.txt : " << dec.size() << " байт, совпадает с исходным: "
         << (dec == big ? "да" : "нет") << "\n"
         << "  коэффициент сжатия K = " << kRatio(big.size(), enc.size()) << "\n";
}

// ===========================================================================
//  ЗАДАНИЯ 2 и 3. LZ77
// ===========================================================================

struct LZ77Token { int offset, length; string next; };

// Кодирование LZ77: в уже просмотренной части текста (буфере поиска)
// отыскивается самое длинное совпадение с началом ещё не закодированной
// части (упреждающего буфера). Результат - тройка (смещение, длина, символ).
vector<LZ77Token> lz77Encode(const vector<string> &s, vector<string> *log = nullptr) {
    vector<LZ77Token> res;
    int n = (int)s.size();
    int pos = 0;
    while (pos < n) {
        int bestLen = 0, bestOff = 0;
        for (int start = 0; start < pos; start++) {       // перебор позиций
            int len = 0;
            while (pos + len < n && s[start + len] == s[pos + len] &&
                   start + len < pos + len + 1) {
                len++;
                if (start + len >= pos + len) break;      // допускается наложение
            }
            // длина ограничена так, чтобы не выйти за конец текста
            while (len > 0 && pos + len > n) len--;
            if (len > bestLen) { bestLen = len; bestOff = pos - start; }
        }
        string next = (pos + bestLen < n) ? s[pos + bestLen] : "";
        res.push_back({bestLen ? bestOff : 0, bestLen, next});
        if (log) {
            ostringstream os;
            os << setw(3) << res.size() << setw(6) << pos << "   " << setw(22)
               << join(vector<string>(s.begin(), s.begin() + pos))
               << "  " << setw(22) << join(vector<string>(s.begin() + pos, s.end()))
               << "   (" << res.back().offset << ", " << res.back().length
               << ", " << (next.empty() ? string("-") : next) << ")";
            log->push_back(os.str());
        }
        pos += bestLen + 1;
    }
    return res;
}

vector<string> lz77Decode(const vector<LZ77Token> &code) {
    vector<string> out;
    for (auto &t : code) {
        int start = (int)out.size() - t.offset;
        for (int i = 0; i < t.length; i++) out.push_back(out[start + i]);
        if (!t.next.empty()) out.push_back(t.next);
    }
    return out;
}

void lz77Report(const string &text, const string &title, int bitsPerSymbol = 8) {
    cout << "\n  " << title << "\n";
    auto s = toChars(text);
    vector<string> log;
    auto code = lz77Encode(s, &log);
    cout << "  исходная строка: " << text << ", длина " << s.size() << " символов\n\n";
    cout << "  шаг   поз.   буфер поиска              упреждающий буфер        код\n";
    for (size_t i = 0; i < log.size(); i++)
        cout << "  " << log[i] << "\n";

    cout << "\n  результат сжатия: ";
    for (auto &t : code)
        cout << "(" << t.offset << "," << t.length << ","
             << (t.next.empty() ? string("-") : t.next) << ") ";
    auto back = lz77Decode(code);
    cout << "\n  восстановленная строка: " << join(back)
         << " (совпадает: " << (join(back) == text ? "да" : "нет") << ")\n";

    // оценка размера: смещение и длина по 5 бит, символ 8 бит
    int bitsSrc = (int)s.size() * bitsPerSymbol;
    int bitsDst = (int)code.size() * (5 + 5 + bitsPerSymbol);
    cout << "  размер исходной строки: " << bitsSrc << " бит, сжатой: "
         << bitsDst << " бит (" << code.size() << " троек по 5 + 5 + "
         << bitsPerSymbol << " бит)\n"
         << "  коэффициент сжатия K = " << fixed << setprecision(3)
         << (double)bitsSrc / bitsDst << "\n";
}

void task2() {
    cout << "\n=== ЗАДАНИЕ 2. LZ77, текст варианта ===\n";
    lz77Report("porpoterpoterporter", "Строка варианта 14");

    // сжатие текстового файла
    ifstream in("rle_in.txt");
    string big((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    if (!big.empty()) {
        auto s = toChars(big.substr(0, 2000));
        auto code = lz77Encode(s);
        auto back = lz77Decode(code);
        int bitsSrc = (int)s.size() * 8, bitsDst = (int)code.size() * 18;
        cout << "\n  Сжатие текстового файла (первые " << s.size() << " символов):\n"
             << "  троек: " << code.size() << ", размер сжатого текста "
             << bitsDst << " бит против " << bitsSrc << " бит\n"
             << "  коэффициент сжатия K = " << (double)bitsSrc / bitsDst
             << ", восстановление точное: " << (join(back) == join(s) ? "да" : "нет")
             << "\n";
    }
}

void task3() {
    cout << "\n=== ЗАДАНИЕ 3. LZ77, двоичный код варианта ===\n";
    lz77Report("0010100110010000001", "Двоичная последовательность варианта 14", 1);
}

// ===========================================================================
//  ЗАДАНИЕ 4. LZ78
// ===========================================================================

struct LZ78Token { int index; string ch; };

vector<LZ78Token> lz78Encode(const vector<string> &s, vector<string> *log = nullptr) {
    map<string,int> dict;                     // фраза -> номер в словаре
    vector<LZ78Token> res;
    string cur;
    for (size_t i = 0; i < s.size(); i++) {
        string next = cur + s[i];
        if (dict.count(next)) { cur = next; continue; }
        int idx = cur.empty() ? 0 : dict[cur];
        res.push_back({idx, s[i]});
        dict[next] = (int)dict.size() + 1;
        if (log) {
            ostringstream os;
            os << setw(4) << res.size() << setw(14) << next
               << setw(10) << dict[next] << "   (" << idx << ", " << s[i] << ")";
            log->push_back(os.str());
        }
        cur.clear();
    }
    if (!cur.empty()) res.push_back({dict[cur], ""});
    return res;
}

vector<string> lz78Decode(const vector<LZ78Token> &code) {
    vector<string> dict(1, "");
    string out;
    for (auto &t : code) {
        string phrase = dict[t.index] + t.ch;
        out += phrase;
        dict.push_back(phrase);
    }
    return toChars(out);
}

void task4() {
    cout << "\n=== ЗАДАНИЕ 4. LZ78, текст варианта ===\n";
    string text = "porpoterpoterporter";
    auto s = toChars(text);
    vector<string> log;
    auto code = lz78Encode(s, &log);

    cout << "  исходная строка: " << text << ", длина " << s.size() << " символов\n\n";
    cout << "   шаг         фраза   номер в словаре   код (индекс, символ)\n";
    for (auto &l : log) cout << "  " << l << "\n";

    cout << "\n  результат сжатия: ";
    for (auto &t : code) cout << "(" << t.index << "," << (t.ch.empty() ? "-" : t.ch) << ") ";
    auto back = lz78Decode(code);
    cout << "\n  восстановленная строка: " << join(back)
         << " (совпадает: " << (join(back) == text ? "да" : "нет") << ")\n";

    int bitsSrc = (int)s.size() * 8;
    int bitsDst = (int)code.size() * (4 + 8);
    cout << "  размер исходной строки " << bitsSrc << " бит, сжатой " << bitsDst
         << " бит (" << code.size() << " пар по 4 + 8 бит)\n"
         << "  коэффициент сжатия K = " << fixed << setprecision(3)
         << (double)bitsSrc / bitsDst << "\n";

    ifstream in("rle_in.txt");
    string big((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    if (!big.empty()) {
        auto bs = toChars(big);
        auto bcode = lz78Encode(bs);
        auto bback = lz78Decode(bcode);
        int bits = 0;
        int dictBits = (int)ceil(log2(max<size_t>(2, bcode.size() + 1)));
        bits = (int)bcode.size() * (dictBits + 8);
        cout << "\n  Сжатие текстового файла:\n"
             << "  исходный файл " << bs.size() << " символов (" << bs.size() * 8
             << " бит), фраз словаря " << bcode.size() << "\n"
             << "  сжатый текст " << bits << " бит, K = "
             << (double)(bs.size() * 8) / bits
             << ", восстановление точное: "
             << (join(bback) == join(bs) ? "да" : "нет") << "\n";
    }
}

// ===========================================================================
//  ЗАДАНИЯ 5 и 6. Префиксные коды: Шеннон-Фано и Хаффман
// ===========================================================================

struct Sym { string ch; int freq; double p; string code; };

// Построение кодов Шеннона-Фано: список символов, упорядоченный по убыванию
// частоты, делится на две части с примерно равными суммами частот; верхней
// части приписывается 0, нижней 1, деление повторяется рекурсивно.
void shannonFano(vector<Sym> &a, int l, int r, vector<string> *log = nullptr) {
    if (r - l < 1) return;
    int total = 0;
    for (int i = l; i <= r; i++) total += a[i].freq;
    int acc = 0, split = l, best = 1 << 30;
    for (int i = l; i < r; i++) {
        acc += a[i].freq;
        int diff = abs(2 * acc - total);
        if (diff < best) { best = diff; split = i; }
    }
    if (log) {
        ostringstream os;
        os << "  группа [";
        for (int i = l; i <= r; i++) os << a[i].ch << (i < r ? " " : "");
        os << "] сумма " << total << " делится после " << a[split].ch;
        log->push_back(os.str());
    }
    for (int i = l; i <= split; i++) a[i].code += "0";
    for (int i = split + 1; i <= r; i++) a[i].code += "1";
    shannonFano(a, l, split, log);
    shannonFano(a, split + 1, r, log);
}

// Дерево Хаффмана: два символа с наименьшими частотами объединяются в узел,
// частота которого равна сумме частот; процесс повторяется, пока не останется
// один узел - корень дерева.
struct HNode {
    int freq;
    string ch;
    HNode *left, *right;
    HNode(int f, string c) : freq(f), ch(c), left(nullptr), right(nullptr) {}
    HNode(HNode *l, HNode *r) : freq(l->freq + r->freq), ch(""), left(l), right(r) {}
};

void huffCodes(HNode *p, string cur, map<string,string> &out) {
    if (!p) return;
    if (!p->left && !p->right) { out[p->ch] = cur.empty() ? "0" : cur; return; }
    huffCodes(p->left, cur + "0", out);
    huffCodes(p->right, cur + "1", out);
}

void huffTree(HNode *p, string prefix, bool isLeft, vector<string> &out) {
    if (!p) return;
    huffTree(p->right, prefix + (isLeft ? "|   " : "    "), false, out);
    out.push_back(prefix + (isLeft ? "\\-- " : "/-- ") +
                  (p->ch.empty() ? "(" + to_string(p->freq) + ")"
                                 : p->ch + " " + to_string(p->freq)));
    huffTree(p->left, prefix + (isLeft ? "    " : "|   "), true, out);
}

// Общий разбор текста: частоты, коды, энтропия, средняя длина, избыточность
void codeReport(const string &text, bool fano) {
    auto chars = toChars(text);
    int n = (int)chars.size();
    map<string,int> freq;
    for (auto &c : chars) freq[c]++;

    vector<Sym> a;
    for (auto &[c, f] : freq) a.push_back({c, f, (double)f / n, ""});
    sort(a.begin(), a.end(), [](const Sym &x, const Sym &y) {
        if (x.freq != y.freq) return x.freq > y.freq;
        return x.ch < y.ch;
    });

    cout << "  текст: " << text << "\n"
         << "  длина текста: " << n << " символов, размер в однобайтовой "
         << "кодировке " << n << " байт = " << n * 8 << " бит\n"
         << "  алфавит текста: " << a.size() << " различных символов\n";
    int h0 = (int)ceil(log2((double)a.size()));
    cout << "  длина равномерного кода символа алфавита H0 = ceil(log2 "
         << a.size() << ") = " << h0 << " бит\n"
         << "  размер текста равномерным кодом: " << n * h0 << " бит\n\n";

    vector<string> log;
    map<string,string> codes;
    HNode *root = nullptr;
    if (fano) {
        shannonFano(a, 0, (int)a.size() - 1, &log);
        for (auto &s : a) codes[s.ch] = s.code;
    } else {
        auto cmp = [](HNode *x, HNode *y) {
            if (x->freq != y->freq) return x->freq > y->freq;
            return x->ch > y->ch;
        };
        priority_queue<HNode*, vector<HNode*>, decltype(cmp)> pq(cmp);
        for (auto &s : a) pq.push(new HNode(s.freq, s.ch));
        while (pq.size() > 1) {
            HNode *x = pq.top(); pq.pop();
            HNode *y = pq.top(); pq.pop();
            log.push_back("  объединяются " +
                (x->ch.empty() ? "узел(" + to_string(x->freq) + ")" : x->ch + "(" + to_string(x->freq) + ")") +
                " и " +
                (y->ch.empty() ? "узел(" + to_string(y->freq) + ")" : y->ch + "(" + to_string(y->freq) + ")") +
                " -> узел(" + to_string(x->freq + y->freq) + ")");
            pq.push(new HNode(x, y));
        }
        root = pq.top();
        huffCodes(root, "", codes);
        for (auto &s : a) s.code = codes[s.ch];
    }

    cout << (fano ? "  процесс деления групп:\n" : "  процесс объединения узлов:\n");
    for (auto &l : log) cout << l << "\n";

    cout << "\n  символ  частота  вероятность   код   длина\n";
    double H = 0, Lavg = 0;
    int bits = 0;
    for (auto &s : a) {
        H -= s.p * log2(s.p);
        Lavg += s.p * s.code.size();
        bits += s.freq * (int)s.code.size();
        cout << "  " << setw(6) << s.ch << setw(9) << s.freq
             << setw(13) << fixed << setprecision(4) << s.p
             << setw(10) << s.code << setw(7) << s.code.size() << "\n";
    }

    if (!fano && root) {
        cout << "\n  дерево кодов (слева направо от корня, в скобках частоты):\n";
        vector<string> tree;
        huffTree(root, "", false, tree);
        for (auto &l : tree) cout << "  " << l << "\n";
    }

    string encoded;
    for (auto &c : chars) encoded += codes[c];

    cout << "\n  закодированный текст (" << encoded.size() << " бит):\n  ";
    for (size_t i = 0; i < encoded.size(); i++) {
        cout << encoded[i];
        if ((i + 1) % 64 == 0) cout << "\n  ";
    }
    cout << "\n\n  энтропия H = " << setprecision(4) << H << " бит на символ\n"
         << "  средняя длина кода Lср = " << Lavg << " бит на символ\n"
         << "  избыточность кода L = 1 - H / Lср = "
         << setprecision(4) << (1 - H / Lavg) << "\n"
         << "  размер сжатого текста: " << bits << " бит ("
         << (bits + 7) / 8 << " байт)\n"
         << "  коэффициент сжатия относительно ASCII (8 бит): "
         << setprecision(3) << (double)(n * 8) / bits << "\n"
         << "  коэффициент сжатия относительно равномерного кода (" << h0
         << " бит): " << (double)(n * h0) / bits << "\n";

    // проверка восстановления
    map<string,string> rev;
    for (auto &[c, code] : codes) rev[code] = c;
    string cur, back;
    for (char b : encoded) {
        cur += b;
        if (rev.count(cur)) { back += rev[cur]; cur.clear(); }
    }
    cout << "  восстановленный текст совпадает с исходным: "
         << (back == text ? "да" : "нет") << "\n";
}

// Сжатие текстового файла префиксными кодами: строятся коды по частотам
// символов файла, текст кодируется в битовую строку, которая упаковывается
// в байты и записывается в файл; заголовок хранит таблицу кодов.
void packFile(const string &inName, const string &outName, bool fano) {
    ifstream in(inName);
    string text((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    if (text.empty()) { cout << "  файл " << inName << " пуст\n"; return; }

    auto chars = toChars(text);
    map<string,int> freq;
    for (auto &c : chars) freq[c]++;
    vector<Sym> a;
    for (auto &[c, f] : freq) a.push_back({c, f, (double)f / chars.size(), ""});
    sort(a.begin(), a.end(), [](const Sym &x, const Sym &y) {
        if (x.freq != y.freq) return x.freq > y.freq;
        return x.ch < y.ch;
    });

    map<string,string> codes;
    if (fano) {
        shannonFano(a, 0, (int)a.size() - 1);
        for (auto &sy : a) codes[sy.ch] = sy.code;
    } else {
        auto cmp = [](HNode *x, HNode *y) {
            if (x->freq != y->freq) return x->freq > y->freq;
            return x->ch > y->ch;
        };
        priority_queue<HNode*, vector<HNode*>, decltype(cmp)> pq(cmp);
        for (auto &sy : a) pq.push(new HNode(sy.freq, sy.ch));
        while (pq.size() > 1) {
            HNode *x = pq.top(); pq.pop();
            HNode *y = pq.top(); pq.pop();
            pq.push(new HNode(x, y));
        }
        huffCodes(pq.top(), "", codes);
    }

    string bits;
    for (auto &c : chars) bits += codes[c];

    // упаковка битовой строки в байты
    string packed;
    for (size_t i = 0; i < bits.size(); i += 8) {
        unsigned char b = 0;
        for (int k = 0; k < 8 && i + k < bits.size(); k++)
            if (bits[i + k] == '1') b |= (unsigned char)(1 << (7 - k));
        packed += (char)b;
    }
    ofstream(outName, ios::binary).write(packed.data(), (long)packed.size());

    // восстановление
    map<string,string> rev;
    for (auto &[c, code] : codes) rev[code] = c;
    string cur, back;
    for (char b : bits) {
        cur += b;
        if (rev.count(cur)) { back += rev[cur]; cur.clear(); }
    }
    ofstream(inName + ".back") << back;

    double lavg = (double)bits.size() / chars.size();
    double H = 0;
    for (auto &sy : a) H -= sy.p * log2(sy.p);
    cout << "\n  Сжатие текстового файла " << inName << " ("
         << (fano ? "Шеннон-Фано" : "Хаффман") << "):\n"
         << "  символов в файле: " << chars.size() << ", алфавит "
         << a.size() << " символов\n"
         << "  размер исходного файла: " << text.size() << " байт\n"
         << "  размер сжатого файла:   " << packed.size() << " байт ("
         << bits.size() << " бит)\n"
         << "  размер восстановленного файла: " << back.size() << " байт, "
         << "совпадает с исходным: " << (back == text ? "да" : "нет") << "\n"
         << "  средняя длина кода Lср = " << fixed << setprecision(4) << lavg
         << " бит, энтропия H = " << H << " бит\n"
         << "  избыточность L = " << setprecision(4) << (1 - H / lavg) << "\n"
         << "  коэффициент сжатия K = " << setprecision(3)
         << (double)text.size() / packed.size() << "\n";
}

// подготовка текстового файла большого размера для заданий 5 и 6
void makeTextFile(const string &name) {
    ofstream f(name);
    string base = "Ана-дэус-рики-паки, Дормы-кормыконсту-таки, "
                  "Энус-дэус-кана-дэусБАЦ! ";
    for (int i = 0; i < 300; i++) f << base;
}

void task5() {
    cout << "\n=== ЗАДАНИЕ 5. Метод Шеннона-Фано ===\n";
    codeReport("Ана-дэус-рики-паки, Дормы-кормыконсту-таки, Энус-дэус-кана-дэусБАЦ!", true);
    makeTextFile("text_big.txt");
    packFile("text_big.txt", "text_fano.bin", true);
}

void task6() {
    cout << "\n=== ЗАДАНИЕ 6. Метод Хаффмана ===\n";
    codeReport("Мирзоян Ашот Оганесович", false);
    makeTextFile("text_big.txt");
    packFile("text_big.txt", "text_huff.bin", false);
}

int main(int argc, char **argv) {
    ios::sync_with_stdio(0);
    if (argc > 1) {
        string a = argv[1];
        if (a == "1") task1();
        else if (a == "2") task2();
        else if (a == "3") task3();
        else if (a == "4") task4();
        else if (a == "5") task5();
        else if (a == "6") task6();
        return 0;
    }
    while (true) {
        cout << "\n---------------------------------------------\n"
             << " Практическая работа 8_1. Сжатие данных. Вариант 14.\n"
             << " 1 - RLE\n 2 - LZ77 (текст)\n 3 - LZ77 (двоичный код)\n"
             << " 4 - LZ78\n 5 - Шеннон-Фано\n 6 - Хаффман\n 0 - выход\n выбор: ";
        int c;
        if (!(cin >> c) || c == 0) break;
        if (c == 1) task1(); else if (c == 2) task2(); else if (c == 3) task3();
        else if (c == 4) task4(); else if (c == 5) task5(); else if (c == 6) task6();
    }
    return 0;
}
