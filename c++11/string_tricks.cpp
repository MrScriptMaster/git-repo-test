/**
 * @file string_tricks.cpp
 * @author Grigory Okhmak (ohmak88@yandex.ru)
 * @details 
 * В этом файле показаны некоторые общие приемы работы со строками.
 * Компилируйте этот файл с флагом --std=c++17 или --std=gnu++17.
 * @version 0.1
 * @date 2019-07-03
 * 
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 */

#include <iostream>
#include <fstream>
#include <iterator>
#include <iomanip>
#include <vector>
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

/*
 * Другой часто встречающейся задачей является токенизация входящего потока строк.
 * Не важно какой поток вы будете использовать, так как в STL работа с ними похожа. Мы
 * рассмотрим пример, когда пользователь вводит строку, в которой используется разделитель
 * в виде запятой (по умолчанию). Мы задействуем стандартный поток ввода, чтобы обрабатывать
 * строку.
 */

void parse_input_stream(char delim = ',')
{
    cout << "Please type something. Delimeter = '" << delim << "'" << endl;
    for (string s; getline(cin >> ws, s, delim);)
    {
        if(s.empty()) break;
        cout << "Parsed line: \"" << s << "\"" << endl;
    }
    /*
     * Чтобы завершить ввод, нужно ввести два раза подряд разделитель.
     * Если во время ввода вы нажмете Enter, то обработается порция данных,
     * кроме самого последнего введенного значения, если вы не введете после него разделитель.
     * 
     * Манипулятор ws для потока cin позволяет отбрасывать начальные пробелы очередного токена.
     */
}

/*
 * На практике часто нужно обрабатывать входные файлы с текстовым наполнением. Типовой задачей
 * является подсчет слов в тексте. Важно в этом примере увидеть не как считать слова, а как можно
 * использовать потоки ввода для разбиения большого текста на смысловые части.
 */

template<typename T>
auto get_count_executor(T& inpStream) {
    return [](T& inpStream) -> size_t {
        return distance(istream_iterator<string>{inpStream}, {});
        /*
         * distance принимает два итератора и возвращает число перемещений с первого
         * на второй. В данном случае количество слов подсчитывается за счет того,
         * что мы передаем итератор потока, который начинает с начала потока, и вторым
         * аргументом итератор end. Так как итератор istream_iterator однонаправленный,
         * получается, что своим перемещением он посчитает количество слов.
         */
    };
}

void word_counter(int argc, char* argv[]) {
    size_t counter = 0;
    /*
     * Обратите внимание, что в большинстве случаев следует предоставлять пользователю возможность
     * выбирать источник ввода. Например, в следующем условии мы проверяем аргументы для программы.
     * Если пользователь передал что-то аргументом, то мы полагаем, что это файл, иначе используем
     * стандартный поток ввода.
     */
    if (argc == 2) {
        ifstream ifs {argv[1]};
        counter = get_count_executor(ifs)(ifs);
    }
    else {
        cout << "Please type text. Press Ctrl+D to stop entering" << endl;
        counter = get_count_executor(cin)(cin);
    }
    cout << "Count result: " << counter << endl;
}

/*
 * На практике из стандартного ввода также требуется инициализировать целые структуры.
 * Обычно для этого перегружают оператор >>.
 */
struct city
{
    string name;
    size_t population;
    double latitude;
    double longitude;
};

istream& operator>>(istream& is, city& city) {
    is >> ws;
    /*
     * Формат десериализации:
     * 
     * name
     * population latitude longitude
     * 
     */
    getline(is, city.name);
    is >> city.population
       >> city.latitude
       >> city.longitude;
       return is;
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
    
    //3
    parse_input_stream();

    //4
    word_counter(argc, argv);

    //5
    vector<city> vCity;
    cout << "Cities:" << endl;
    // Читаем структуры из потока
    /*
     * Всю десериализацию структуры выполняет итератор. В случае, если ему это не
     * удается, то поток переходит в ошибочное состояние, которая останавливает работу
     * алгоритма copy.
     */
    stringstream ss;
    ss << "Malinovka\n12300 125.21 65.34\n"
       << "Petrovka 38\n45688 125.5 .44\n"
       << "Bad format\n"
       << "Vavilovka 4654 .123 .432";
    copy(istream_iterator<city>{ss}, {}, back_inserter(vCity));
    for (const auto &[name, pop, lat, lon] : vCity) {
        cout << left << setw(15)
            << name
            << " population=" << pop
            << " lat=" << lat
            << " lon=" << lon << endl;
    }

    return 0;
}