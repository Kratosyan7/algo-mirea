// ============================================================================
//  Практическая работа 6_2. Поиск в тексте образца. Вариант 14.
//
//  Задание 1. Дан текст из слов, разделённых пробелами, и подстрока.
//             Сформировать список слов, в которых подстрока размещается
//             в конце слова. Только посимвольный перебор.
//  Задание 2. Найти максимальную по длине подстроку текста, являющуюся
//             палиндромом. Метод: алгоритм Манакера.
//  Задание 3. Дан текст с именами файлов через запятую. Найти имена
//             текстовых файлов, у которых последний символ имени - цифра,
//             а первый символ - заданная буква. Регулярные выражения.
//
//  Сборка: g++ -std=c++17 -O2 -Wall main.cpp -o text
//  Запуск: ./text        - меню
//          ./text 1|2|3  - демонстрация задания
//          ./text b1|b2  - замеры времени для задания 1 или 2
// ============================================================================

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <regex>
#include <chrono>
#include <random>
#include <cstdio>

using namespace std;
using namespace chrono;

typedef long long ll;

ll cmpCount = 0;          // число сравнений символов

// ===========================================================================
//  ЗАДАНИЕ 1. Линейный (наивный) поиск подстроки и отбор слов
// ===========================================================================

// Наивный поиск: возвращает позицию первого вхождения pat в s начиная с
// позиции from, или -1. Сравнение только посимвольное.
ll naiveSearch(const string &s, const string &pat, ll from = 0) {
    ll n = s.size(), m = pat.size();
    if (m == 0 || m > n) return -1;
    for (ll i = from; i + m <= n; i++) {
        ll j = 0;
        while (j < m) {
            cmpCount++;
            if (s[i + j] != pat[j]) break;
            j++;
        }
        if (j == m) return i;
    }
    return -1;
}

// Проверка, что pat является суффиксом слова w (посимвольное сравнение
// с конца слова).
bool endsWith(const string &w, const string &pat) {
    ll n = w.size(), m = pat.size();
    if (m == 0 || m > n) return false;
    for (ll j = 0; j < m; j++) {
        cmpCount++;
        if (w[n - m + j] != pat[j]) return false;
    }
    return true;
}

// Разбор текста на слова без библиотечных функций поиска: слово - это
// максимальная последовательность символов между пробелами.
vector<string> splitWords(const string &text) {
    vector<string> words;
    string cur;
    for (char c : text) {
        if (c == ' ' || c == '\n' || c == '\t') {
            if (!cur.empty()) { words.push_back(cur); cur.clear(); }
        } else cur += c;
    }
    if (!cur.empty()) words.push_back(cur);
    return words;
}

// Задание 1, вариант А: список слов, в конце которых размещена подстрока pat.
// Проверяется только суффикс слова, поэтому на каждое слово тратится не
// более m сравнений.
vector<string> wordsEndingWith(const string &text, const string &pat) {
    vector<string> res;
    for (auto &w : splitWords(text))
        if (endsWith(w, pat)) res.push_back(w);
    return res;
}

// Задание 1, вариант Б: то же самое полным линейным поиском. В каждом слове
// перебираются все позиции, и слово принимается, если найдено вхождение,
// которое заканчивается последним символом слова. Этот вариант выполняет
// работу O(n * m) и используется для сравнения в замерах.
vector<string> wordsEndingWithScan(const string &text, const string &pat) {
    vector<string> res;
    ll m = pat.size();
    for (auto &w : splitWords(text)) {
        ll from = 0, pos;
        bool ok = false;
        while ((pos = naiveSearch(w, pat, from)) >= 0) {
            if (pos + m == (ll)w.size()) { ok = true; break; }
            from = pos + 1;
        }
        if (ok) res.push_back(w);
    }
    return res;
}

// ===========================================================================
//  ЗАДАНИЕ 2. Алгоритм Манакера: наибольшая подстрока-палиндром
// ===========================================================================

// Преобразование строки: между всеми символами и по краям вставляется
// разделитель '#', благодаря чему палиндромы чётной и нечётной длины
// обрабатываются одинаково: "aba" -> "#a#b#a#".
string addSeparators(const string &s) {
    string t = "#";
    for (char c : s) { t += c; t += '#'; }
    return t;
}

// Алгоритм Манакера, сложность O(n). Возвращает наибольшую подстроку-
// палиндром; в d возвращается массив радиусов палиндромов.
string manacher(const string &s, vector<int> &d) {
    if (s.empty()) return "";
    string t = addSeparators(s);
    ll n = t.size();
    d.assign(n, 0);

    ll l = 0, r = -1;              // границы самого правого палиндрома
    for (ll i = 0; i < n; i++) {
        // начальный радиус берётся из зеркального центра
        ll k = (i > r) ? 1 : min((ll)d[l + r - i], r - i + 1);
        while (i - k >= 0 && i + k < n) {
            cmpCount++;
            if (t[i - k] != t[i + k]) break;
            k++;
        }
        d[i] = (int)k;
        if (i + k - 1 > r) { l = i - k + 1; r = i + k - 1; }
    }

    ll best = 0, center = 0;
    for (ll i = 0; i < n; i++)
        if (d[i] - 1 > best) { best = d[i] - 1; center = i; }

    ll start = (center - best) / 2;     // пересчёт позиции в исходную строку
    return s.substr(start, best);
}

// Наивный поиск наибольшего палиндрома, сложность O(n^2), для сравнения
string naivePalindrome(const string &s) {
    ll n = s.size();
    ll bestLen = 0, bestPos = 0;
    for (ll c = 0; c < n; c++) {
        for (int odd = 0; odd < 2; odd++) {
            ll a = c, b = c + odd;
            while (a >= 0 && b < n) {
                cmpCount++;
                if (s[a] != s[b]) break;
                a--; b++;
            }
            ll len = b - a - 1;
            if (len > bestLen) { bestLen = len; bestPos = a + 1; }
        }
    }
    return s.substr(bestPos, bestLen);
}

// ===========================================================================
//  ЗАДАНИЕ 3. Регулярные выражения: имена текстовых файлов
// ===========================================================================

// Строит регулярное выражение для имён текстовых файлов, начинающихся
// заданной буквой и оканчивающихся цифрой перед расширением .txt
regex buildFileRegex(char letter) {
    string up(1, (char)toupper((unsigned char)letter));
    string lo(1, (char)tolower((unsigned char)letter));
    string pat = "[" + up + lo + "][A-Za-z0-9_-]*[0-9]\\.txt";
    return regex(pat);
}

vector<string> findTextFiles(const string &text, char letter) {
    regex re = buildFileRegex(letter);
    vector<string> res;
    for (sregex_iterator it(text.begin(), text.end(), re), end; it != end; ++it)
        res.push_back(it->str());
    return res;
}

// Проверка одного имени на соответствие шаблону целиком
bool matchFileName(const string &name, char letter) {
    return regex_match(name, buildFileRegex(letter));
}

// ===========================================================================
//  Демонстрация заданий
// ===========================================================================

void demo1() {
    cout << "\n=== ЗАДАНИЕ 1. Слова, оканчивающиеся заданной подстрокой ===\n";
    struct Test { string text, pat, note; };
    vector<Test> tests = {
        {"privet kompot bilet paket salut", "et",
         "образец в конце нескольких слов"},
        {"eto test testovyy testet", "test",
         "образец в начале, середине и конце слова"},
        {"mama myla ramu", "xyz", "образца в тексте нет"},
        {"abc", "abc", "слово целиком совпадает с образцом"},
        {"aaa aaaa aa a", "aa", "перекрывающиеся совпадения"},
    };
    for (auto &t : tests) {
        cmpCount = 0;
        auto res = wordsEndingWith(t.text, t.pat);
        cout << "\n  текст   : \"" << t.text << "\"\n"
             << "  образец : \"" << t.pat << "\"  (" << t.note << ")\n"
             << "  результат: ";
        if (res.empty()) cout << "слов не найдено";
        for (size_t i = 0; i < res.size(); i++)
            cout << (i ? ", " : "") << res[i];
        cout << "\n  сравнений символов: " << cmpCount << "\n";
    }

    cout << "\n  Наивный поиск позиции образца в тексте:\n";
    string text = "poisk obrazca v tekste: obrazec obrazca";
    cmpCount = 0;
    ll pos = naiveSearch(text, "obrazca");
    cout << "  первое вхождение \"obrazca\" в позиции " << pos
         << ", сравнений " << cmpCount << "\n";
}

void demo2() {
    cout << "\n=== ЗАДАНИЕ 2. Наибольший палиндром (алгоритм Манакера) ===\n";
    vector<pair<string,string>> tests = {
        {"abacaba", "весь текст является палиндромом"},
        {"forgeeksskeegfor", "палиндром чётной длины в середине"},
        {"abcdefg", "палиндромов длиннее одного символа нет"},
        {"aaaaa", "все символы одинаковы"},
        {"babadada", "несколько палиндромов, нужен наибольший"},
    };
    vector<int> d;
    for (auto &t : tests) {
        cmpCount = 0;
        string res = manacher(t.first, d);
        ll c1 = cmpCount;
        cmpCount = 0;
        string res2 = naivePalindrome(t.first);
        cout << "\n  текст: \"" << t.first << "\"  (" << t.second << ")\n"
             << "  Манакер: \"" << res << "\", длина " << res.size()
             << ", сравнений " << c1 << "\n"
             << "  наивный: \"" << res2 << "\", сравнений " << cmpCount << "\n";
    }

    cout << "\n  Массив радиусов d для строки \"abacaba\":\n  ";
    manacher("abacaba", d);
    string t = addSeparators("abacaba");
    for (size_t i = 0; i < t.size(); i++) cout << setw(3) << t[i];
    cout << "\n  ";
    for (size_t i = 0; i < d.size(); i++) cout << setw(3) << d[i];
    cout << "\n";
}

void demo3() {
    cout << "\n=== ЗАДАНИЕ 3. Регулярные выражения: имена файлов ===\n";
    string text =
        "otchet1.txt, doc2.doc, otchet.txt, obzor7.txt, ob3.txt, "
        "Otchet9.txt, oq.txt, otchet12.txt, other5.dat, o_file4.txt";
    char letter = 'o';
    cout << "  текст: " << text << "\n"
         << "  заданная буква: '" << letter << "'\n"
         << "  регулярное выражение: [Oo][A-Za-z0-9_-]*[0-9]\\.txt\n\n";

    auto res = findTextFiles(text, letter);
    cout << "  найдено имён: " << res.size() << "\n";
    for (auto &s : res) cout << "    " << s << "\n";

    cout << "\n  Проверка отдельных имён (regex_match):\n";
    vector<string> names = {"otchet1.txt", "otchet.txt", "doc2.txt",
                            "obzor7.doc", "O12.txt", "o-file9.txt", "o9.txtx"};
    for (auto &n : names)
        cout << "    " << setw(14) << left << n << right << " -> "
             << (matchFileName(n, letter) ? "соответствует" : "не соответствует")
             << "\n";
}

// ===========================================================================
//  Замеры времени
// ===========================================================================

// Текст из слов длиной wlen, разделённых пробелами. При worst = true все
// символы одинаковы ('a'), что вместе с образцом вида "aaaab" даёт худший
// случай: каждое сравнение доходит до последнего символа образца.
string genWords(ll n, ll wlen, bool worst) {
    string s;
    s.reserve(n);
    mt19937 rnd(20260930);
    for (ll i = 0; i < n; i++) {
        if (i % (wlen + 1) == wlen) s += ' ';
        else s += worst ? 'a' : (char)('a' + rnd() % 26);
    }
    return s;
}

void bench1() {
    cout << "\n=== Задание 1: время поиска (формат табл. 21) ===\n";
    printf("%10s %4s %13s %12s %13s %12s %13s %12s\n", "n", "m",
           "T суфф, мс", "Тф суфф", "T худш, мс", "Тф худш",
           "T полн, мс", "Тф полн");
    for (ll n : {100LL, 1000LL, 10000LL, 100000LL, 1000000LL}) {
        ll m = 5, wlen = 20;                      // длина слова 20 символов
        string best = genWords(n, wlen, false);
        string worst = genWords(n, wlen, true);
        string pat(m - 1, 'a'); pat += 'b';       // худший образец

        int rep = n <= 10000 ? 200 : 5;
        cmpCount = 0;
        auto t0 = steady_clock::now();
        for (int i = 0; i < rep; i++) wordsEndingWith(best, "abcde");
        auto t1 = steady_clock::now();
        double tb = duration<double, milli>(t1 - t0).count() / rep;
        ll cb = cmpCount / rep;

        cmpCount = 0;
        t0 = steady_clock::now();
        for (int i = 0; i < rep; i++) wordsEndingWith(worst, pat);
        t1 = steady_clock::now();
        double tw = duration<double, milli>(t1 - t0).count() / rep;
        ll cw = cmpCount / rep;

        cmpCount = 0;
        t0 = steady_clock::now();
        for (int i = 0; i < rep; i++) wordsEndingWithScan(worst, pat);
        t1 = steady_clock::now();
        double ts = duration<double, milli>(t1 - t0).count() / rep;
        ll cs = cmpCount / rep;

        printf("%10lld %4lld %13.4f %12lld %13.4f %12lld %13.4f %12lld\n",
               n, m, tb, cb, tw, cw, ts, cs);
    }
}

void bench2() {
    cout << "\n=== Задание 2: время поиска палиндрома (формат табл. 21) ===\n";
    cout << "  Текст 1: случайные символы из четырёх букв (обычный случай).\n"
         << "  Текст 2: все символы одинаковы (худший случай наивного метода,\n"
         << "           весь текст является палиндромом).\n\n";
    printf("%9s %8s %11s %12s %11s %12s | %11s %12s %11s %12s\n",
           "n", "длина", "Ман сл, мс", "Тф Ман сл", "наив сл, мс", "Тф наив сл",
           "Ман од, мс", "Тф Ман од", "наив од, мс", "Тф наив од");
    mt19937 rnd(20260930);
    vector<int> d;
    for (ll n : {100LL, 1000LL, 10000LL, 100000LL, 1000000LL}) {
        string rnd_s, same_s(n, 'a');
        rnd_s.reserve(n);
        for (ll i = 0; i < n; i++) rnd_s += (char)('a' + rnd() % 4);

        int rep = n <= 10000 ? 100 : 3;
        auto measure = [&](const string &s, bool useManacher, double &t, ll &c) {
            cmpCount = 0;
            auto t0 = steady_clock::now();
            string res;
            for (int i = 0; i < rep; i++)
                res = useManacher ? manacher(s, d) : naivePalindrome(s);
            auto t1 = steady_clock::now();
            t = duration<double, milli>(t1 - t0).count() / rep;
            c = cmpCount / rep;
            return res;
        };

        double t1m, t1n, t2m, t2n;
        ll c1m, c1n, c2m, c2n;
        string res = measure(rnd_s, true, t1m, c1m);
        measure(rnd_s, false, t1n, c1n);
        measure(same_s, true, t2m, c2m);
        if (n <= 100000) measure(same_s, false, t2n, c2n);
        else { t2n = -1; c2n = -1; }             // на 10^6 наивный слишком долог

        printf("%9lld %8zu %11.4f %12lld %11.4f %12lld | %11.4f %12lld %11.4f %12lld\n",
               n, res.size(), t1m, c1m, t1n, c1n, t2m, c2m, t2n, c2n);
    }
}

int main(int argc, char **argv) {
    ios::sync_with_stdio(0);
    if (argc > 1) {
        string a = argv[1];
        if (a == "1") demo1();
        else if (a == "2") demo2();
        else if (a == "3") demo3();
        else if (a == "b1") bench1();
        else if (a == "b2") bench2();
        return 0;
    }
    while (true) {
        cout << "\n---------------------------------------------\n"
             << " Практическая работа 6_2. Вариант 14.\n"
             << " 1 - задание 1: слова с подстрокой в конце\n"
             << " 2 - задание 2: наибольший палиндром (Манакер)\n"
             << " 3 - задание 3: регулярные выражения\n"
             << " 4 - замеры времени для задания 1\n"
             << " 5 - замеры времени для задания 2\n"
             << " 0 - выход\n выбор: ";
        int k;
        if (!(cin >> k) || k == 0) break;
        if (k == 1) demo1();
        else if (k == 2) demo2();
        else if (k == 3) demo3();
        else if (k == 4) bench1();
        else if (k == 5) bench2();
    }
    return 0;
}
