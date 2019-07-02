/**
 * @file string_tricks.cpp
 * @author Grigory Okhmak (ohmak88@yandex.ru)
 * @details 
 * В этом файле показаны некоторые общие приемы работы со строками.
 * @version 0.1
 * @date 2019-07-03
 * 
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <string_view>

using namespace std;

/*
 * Часто на практике нужно подравнивать строки, т.е. удалять пробелы вначале и
 * в конце. Ниже показана типовая функция для реализации подравнивания с любой
 * стороны.
 */

enum eSide {
    eLEFT = 0,
    eRIGHT,
    eSURROUND
};

string trim(const string &s, eSide where = eSURROUND)
{
    const char whitespace[] {" \t\n"};
    const size_t first(s.find_first_not_of(whitespace));
    if (string::npos == first) { return {}; }
    if (where == eLEFT) { return s.substr(first,s.size()); }
    const size_t last(s.find_last_not_of(whitespace));
    if (where == eRIGHT) { return s.substr(0, last + 1); }
    return s.substr(first, (last - first + 1));
}

/*
 * В стандарте с++17 появился такой класс как string_view, который работает со
 * строками ссылаясь на существующий фрагмент в памяти, а не конструируя новую
 * строку. Это позволяет ускорить работу со строками, если, например, их не нужно
 * изменять, а нужно только выводить.
 * 
 * Следующая реализация функции trim работает быстрее предыдущей, за счет
 * того, что мы работаем с уже существующими в памяти строками.
 */

string_view& fast_trim(string_view&& v, eSide where = eSURROUND)
{
    if (where == eSURROUND || where == eLEFT)
    {
        const auto first(v.find_first_not_of(" \t\n"));
        v.remove_prefix(min(first, v.size()));
    }
    if (where == eSURROUND || where == eRIGHT)
    {
        const auto last(v.find_last_not_of(" \t\n"));
        if (last != string_view::npos)
        {
            v.remove_suffix(v.size() - last - 1);
        }
    }
    return v;
}

int main(int argc, char* argv[])
{
    //1
    string test1 {" \t\n hello world \t\n "};
    string test2 {""};
    cout << "|" << trim(test1, eLEFT) << "|" << endl;
    cout << "|" << trim(test1, eRIGHT) << "|" << endl;
    cout << "|" << trim(test1) << "|" << endl;
    cout << "|" << trim(test2, eLEFT) << "|" << endl;
    cout << "|" << trim(test2, eRIGHT) << "|" << endl;
    cout << "|" << trim(test2) << "|" << endl;
        
    //2
    cout << "|" << fast_trim(argv[0]) << "|" << endl;
    cout << "|" << fast_trim({}) << "|" << endl;
    cout << "|" << fast_trim("alpha beta gamma") << "|" << endl;
    cout << "|" << fast_trim(" \t\n alpha beta gamma  \t\n   "sv) << "|" << endl;
    cout << "|" << fast_trim("alpha beta gamma   "s) << "|" << endl;
    char c_str[] {'a', 'b', 'c'};
    cout << "|" << fast_trim(string_view(c_str, sizeof(c_str))) << "|" << endl;
    
    return 0;
}