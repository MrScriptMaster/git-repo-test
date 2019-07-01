
/**
 * @file std_manips.cpp
 * @author Grigory Okhmak (ohmak88@yandex.ru)
 * @details 
 * В этом файле показано как создавать манипуляторы, совместимые со стандартными потоками ввода/вывода.
 * @version 0.1
 * @date 2019-07-01
 * 
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 */

    /*
     * Библиотечные манипуляторы заголовка <iomanip> можно перегрузить, чтобы
     * сделать ваши манипуляторы совместимыми со стандартными потоками.
     * Для этого используются шаблоны imanip<> и omanip<>. К сожалению, пока не всех реализациях
     * компиляторов есть эти шаблоны, но вы можете объявить похожие шаблоны самостоятельно, если их нет.
     */

// --- cut off ---------------------------------------------------- cut off ---

//#pragma once

#include <iostream>
#include <limits>
#include <locale>

namespace NCustomManipulators {

    /*
     * In manipulators
     */
    template <typename> class imanip;
    
    template <typename T>
    inline std::istream& operator>>(std::istream& i,const imanip<T>& m) { return (*m.func)(i,m.obj); }

    /**
     * @brief In-manipulator declaration.
     */
    template <typename T>
    class imanip {
        std::istream& (*func)(std::istream&, T);
        T obj;
    public:
        imanip(std::istream& (*f)(std::istream&, T), T a): func(f), obj(a) {}

        friend std::istream& operator>> <>(std::istream&,const imanip<T>&);
    };

    /*
     * Out manipulators
     */
    template <typename> class omanip;

    template <typename T>
    inline std::ostream& operator<<(std::ostream& i,const omanip<T>& m) { return (*m.func)(i,m.obj); }

    /**
     * @brief Out-manipulator declaration.
     */
    template <typename T>
    class omanip {
        std::ostream& (*func)(std::ostream&,T);
        T obj;
    public:
        omanip(std::ostream& (*f)(std::ostream&,T),T a): func(f), obj(a) {}
        
        friend std::ostream& operator<< <>(std::ostream&,const omanip<T>&);
    };

    /*
     * Some useful manipulators.
     */

    //  Ignoring the istream till the end of the line.

	inline std::istream&
	skip_line(std::istream& is) {
		return is.ignore(std::numeric_limits<int>::max(),'\n');
	}

	//	Eat pure white spaces (' ' and '\t')

	inline std::istream&
	skip_spaces(std::istream& is) {

		char separator;

		while (is.get(separator) && ((separator==' ') || (separator=='\t')));
		return is.putback(separator);
	}

	//  Eat up comments starting with the given character.
	//	Example: To remove the lines begining with a '#' then read data.
	//		cin >> skip_comments('#') >> data;


    inline std::istream&
    skip_lines_internal(std::istream& is,const unsigned char c) {
        is >> std::ws;
        while (is.peek()==c) {
            skip_line(is);
            is >> std::ws;
        }
        return is;
    }

	inline imanip<const unsigned char>
	skip_comments(const unsigned char c) {
		return imanip<const unsigned char>(skip_lines_internal,c);
	}

    //	Test if the input character stream is equal to s.

    inline bool
    match_string(std::istream& is,const char*& s)
    {
        is >> std::ws;
        while (*s && is.peek()==static_cast<const unsigned char>(*s)) {
            is.get();
            s++;
        }

        return *s=='\0';
    }

	//  Eat up comments beginning with the given string.
	//	Example: To remove the lines begining with a "//" then read data.
	//		cin >> skip_comments("//") >> data;

    inline std::istream&
    skip_lines_internal(std::istream& is,const char* s)
    {
        const char* s1 = s;
        while (match_string(is,s1)) {
            skip_line(is);
            s1 = s;
        }

        return is;
    }

	inline imanip<const char*>
	skip_comments(const char* s) {
		return imanip<const char*>(skip_lines_internal,s);
	}

	//	Match the given string with an input stream.
	//	Example: To parse lines such as "x=156 y=34".
	//		cin >> match("x=") >> x >> match("y=") >> y;

    inline std::istream&
    match_internal(std::istream& is,const char* s) {
        is >> std::ws;
        if (!match_string(is,s))
            is.setstate(std::ios::failbit);

        return is;
    }

    inline std::istream&
    match_internal(std::istream& is,const char c) {
        char cc;
        is >> cc;
        if (cc!=c)
            is.setstate(std::ios::failbit);

        return is;
    }

    inline std::istream&
    match_internal(std::istream& is,const int i) {
        int num;
        is >> num;
        if (i!=num)
            is.setstate(std::ios::failbit);

        return is;
    }

    inline std::istream&
    match_internal(std::istream& is,const unsigned i) {
        unsigned num;
        is >> num;
        if (i!=num)
            is.setstate(std::ios::failbit);

        return is;
    }

	inline imanip<const char*>
	match(const char* str) {
		return imanip<const char*>(match_internal,str);
	}

    inline imanip<const char*>
	match(const std::string& str) {
		return imanip<const char*>(match_internal,str.c_str());
	}

    inline imanip<const char>
    match(const char c) {
        return imanip<const char>(match_internal,c);
    }

    inline imanip<const int>
    match(const int i) {
        return imanip<const int>(match_internal,i);
    }

    inline imanip<const unsigned>
    match(const unsigned i) {
        return imanip<const unsigned>(match_internal,i);
    }

    //	Restore the string [s,s1[ to the input stream.

    inline void
    restore_string(std::istream& is,const char* s,const char *s1) {
        while (s!=s1--)
            is.putback(*s1);
    }

    //  The string equivalent.

    inline void
    restore_string(std::istream& is,const std::string& s) {
        restore_string(is,&*s.begin(),&*s.end());
    }

    //  Like match but the matching is optional and the
    //  presence of the string is given in result.

    inline std::istream&
    match_string_internal(std::istream& is,const char* s,bool& res)
    {
        std::ios::iostate state = is.rdstate();
        is >> std::ws;
        const char* s1 = s;
        res = match_string(is,s1);
        if (!res) {
            is.setstate(state);
            restore_string(is,s,s1);
        }

        return is;
    }

    //	Eat the given string if present on an input stream.
    //	Example: To parse lines such as "x=156 y=34".
    //		cin >> eat("x=") >> x >> eat("y=") >> y;
    //	The difference with match is that it will silently
    //	ignore the strings if they are not on the stream
    //	and the stream will remain in the good state.
    //	So 156 34 will also be accepted as an input line.
    //	Note that x156 34 or any other variation will not
    //	be accepted.
    inline std::istream&
    eat_string_internal(std::istream& is,const char* s)
    {
        is >> std::ws;
        const char* s1 = s;
        if (!match_string(is,s1))
            restore_string(is,s,s1);

        return is;
    }

    inline imanip<const char*>
    eat(const char* str) {
        return imanip<const char*>(eat_string_internal,str);
    }

    inline imanip<const char*>
    eat(const std::string& str) {
        return imanip<const char*>(eat_string_internal,str.c_str());
    }

    //	Skip everything until string s is found on the stream.

    inline std::istream&
    skip_to_internal(std::istream& is,const char* s)
    {
        const char* s1 = s;

        while(is) {
            is.ignore(std::numeric_limits<int>::max(),*s);
            if (match_string(is,++s1))
                break;
            restore_string(is,s+1,s1);
            s1 = s;
        }

        return is;
    }

    inline imanip<const char*>
    skip_to(const char* str) {
        return imanip<const char*>(skip_to_internal,str);
    }

    inline imanip<const char*>
    skip_to(const std::string& str) {
        return imanip<const char*>(skip_to_internal,str.c_str());
    }

    //  An optional input is a field eventually put before the
    //  end of the line.

    template <typename T>
    inline std::istream&
    optional_internal(std::istream& is,T& t) {

        skip_spaces(is);
        if (is.peek()!='\n')
            is >> t;
        return is;
    }

    template <typename T>
    inline imanip<T&>
    optional(T& t) {
        return imanip<T&>(optional_internal,t);
    }

    //  Force numeric reading.

    template <typename T>
    inline std::istream&
    numeric_internal(std::istream& is,T& t) {
        return is >> t;
    }

    inline std::istream&
    numeric_internal(std::istream& is,char& t) {
        int i;
        is >> i;
        t = static_cast<char>(i);
        return is;
    }

    inline std::istream&
    numeric_internal(std::istream& is,unsigned char& t) {
        unsigned i;
        is >> i;
        t = static_cast<unsigned char>(i);
        return is;
    }

    template <typename T>
    inline imanip<T&>
    numeric(T& t) {
        return imanip<T&>(numeric_internal,t);
    }

    //  File reading.

    inline std::istream&
    file_internal(std::istream& is,std::string& filename,const char delimiter,const bool force_delimiter) {
        skip_spaces(is);

        //  Read the initial delimiter if necessary.

        char c;
        bool delimiter_seen = false;

        if (is.peek()!=delimiter) {
            if (force_delimiter) {
                is.setstate(std::ios::failbit);
                return is;
            }
        } else {
            is.get(c);
            delimiter_seen = true;
        }

        //  Read the string content.

        while (is.get(c) && c!=delimiter) {
            if (delimiter_seen && c==delimiter)
                break;
            if (!delimiter_seen && std::isspace(c,is.getloc()))
                break;
            filename += c;
        }

        return is;
    }

}

// --- cut off ---------------------------------------------------- cut off ---

#include <iomanip>
#include <cstring>
#include <cstdlib>

namespace NCustomManipulators {
    /* 
     * В реализации NCustomManipulators отсутствуют манипуляторы вывода, потому что обычно сложно
     * придумать какие-то общие. Но мы можем использовать шаблонный класс, чтобы реализовать
     * совместимый манипулятор вывода под наши задачи. Например, мы можем написать собственный выравниватель.
     */
    inline std::ostream&
    indent(std::ostream &stream, int length)
    {
        register int i;
        for (i=0; i < length; i++) std::cout << " ";
        return stream;
    }
    
    /*
     * Длина указывает сколько пробелов нужно вставить вперед.
     */

    omanip<int> indent(int length)
    {
        return omanip<int>(indent, length);
    }

    /*
     * Можно придумать и более практичный манипулятор. Например, пусть следующий манипулятор
     * добавляет знак доллара к числу.
     */

    inline std::ostream&
    dollars(std::ostream &stream, double amount)
    {
        stream.setf (std::ios::showpoint);
        stream << "$" << std::setw(6) << std::setprecision(5) << amount;
        return stream;
    }
    
    omanip <double> dollars(double amount) 
    {
        return omanip<double> (dollars, amount);
    }
}

const char g_secret_password[] = "password";
char g_pass_string[60];

namespace NCustomManipulators {
    /*
     * Можно придумать также манипуляторы ввода, облегчающие работу со входными данными.
     * Например, следующий манипулятор проверяет введенный пароль. В случае определенного числа неудач,
     * программа завершается.
     */
    std::istream &getpass(std::istream &stream, int tries)
    {
        do {
            std::cout << "Enter password: ";
            stream >> g_pass_string;
            if (!strcmp(g_secret_password, g_pass_string) ) return stream;
            std::cout << "\a"; // bell
            tries--;
        } while (tries > 0);
        std::cout << "All tries failed! \n";
        exit(1); // пароль не введен
        return stream;
    }
    
    imanip<int> getpass(int tries) {
        return imanip<int>(getpass, tries);
    }   
}

int main(int argc, char* argv[]) 
{
    //1
    /* Протестируем наши манипуляторы вывода. Обратите внимание, что стандартные манипуляторы работают
     * в гармонии с нашими.
     */
    std::cout << "Test 1:" << std::endl 
              << NCustomManipulators::indent(10) << "Hello" << std::endl 
              // хотя можно было бы тоже самое сделать и так
              << std::setw(10) << std::setfill(' ') << ' ' << std::setw(0) << "Hello again" << std::endl 
              << "I have " << NCustomManipulators::dollars(105.65) << " in my pocket" << std::endl;

    //2
    std::cin >> NCustomManipulators::getpass(3);
    std::cout << "Authentication was successfull" << std::endl;

    //3
    /* Пробуем другие манипуляторы ввода
     * Некоторые из предложенных выше манипуляторов ввода предлагают некоторые
     * типовые дейстаия, которые могут вам пригодиться, например, чтобы парсить файлы.
     */

    return 0;
}