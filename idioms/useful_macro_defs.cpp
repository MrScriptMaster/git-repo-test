/**
 * @file useful_macro_defs.cpp
 * @author Grigory Okhmak (ohmak88@yandex.ru)
 * @details 
 * В этом файле приведены некоторые полезные макросы, позволяющие сэкономить
 * немного времени на написание прототипов.
 * @version 0.1
 * @date 2019-02-05
 * 
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 * 
 */

/**
 * @brief Данный макрос позволяет конструировать строки мз литералов. Им
 * рекомендуется пользоваться, когда некоторые макросы разрешаются строками,
 * приходящими во время вызова компилятора.
 * 
 * @example 
 * @code 
 * "Hello, "__macro_make_string(World!)
 * @endcode
 */
#define __macro_make_string__(arg) #arg
#define __macro_make_string(arg) __macro_make_string__(arg)

/**
 * @brief Данный макрос позволяет конкатенировать два литерала.
 * 
 * @example
 * @code
 * __macro_join(Hello, World!)
 * @endcode
 */
#define __macro_join__(arg1,arg2) arg1##arg2
#define __macro_join(arg1,arg2) __macro_join__(arg1,arg2)

/**
 * @brief Данный макрос позволяет быстро создавать атрибуты вместе с
 * геттерами и сеттерами для классов и структур.
 * 
 * @example
 * @code
 * class CTest {
 *     __macro_make_class_attr(int, Number)
 *     __macro_make_class_attr(std::string, String)
 * };
 * @endcode
 */
#define __macro_make_class_attr(type,name) \
private: type m_##name;\
public:\
    const type& get##name() const { return m_##name; }\
    void set##name(const type& value) { m_##name = value; }

#ifndef __STDC_VERSION__
#define __STDC_VERSION__ -1//__macro_make_string("__STDC_VERSION__ is not supported")
#endif

#ifndef __STDC_NO_COMPLEX__
#define __STDC_NO_COMPLEX__ -1//__macro_make_string("__STDC_NO_COMPLEX__ is not supported")
#endif

#ifndef __STDC_NO_VLA__
#define __STDC_NO_VLA__ -1//__macro_make_string("__STDC_NO_VLA__ is not supported")
#endif

#ifndef __VA_ARGS__
#define __VA_ARGS__ -1//__macro_make_string("__VA_ARGS__ is not supported")
#endif

#ifndef __STDC_IEC_559__
#define __STDC_IEC_559__ -1
#endif

#ifndef __STDC_IEC_559_COMPLEX__
#define __STDC_IEC_559_COMPLEX__ -1
#endif

// Макросы для формирования перечислений 
#ifndef delimiter 
    #define delimiter , 
#endif 
 
#ifndef enumeration_begin 
    #define enumeration_begin(arg) enum arg { 
#endif 
 
#ifndef enumeration_end 
    #ifdef last_enumerator 
        #define enumeration_end delimiter last_enumerator } 
    #else 
        #define enumeration_end } 
    #endif 
#endif 
 
#ifndef declare_member 
    #define declare_member(arg) arg 
#endif 
 
#ifndef member_value 
    #define member_value(arg) = arg 
#endif 

#define NDEBUG

#include <cassert>
#include <iostream>
#include <string>

class TryMacros {
    __macro_make_class_attr(int, Number)
    __macro_make_class_attr(std::string, String)
public:
    TryMacros(int number, const std::string& string)
        : m_Number(number)
        ,m_String(string)
    {}
};

using namespace std;

void function() {
    cout << "Processed on " << __LINE__ << " line" << endl;
    cout << "Processed in function named by " << __FUNCTION__ << endl;
    cout << "COUNTER IS " << __COUNTER__ << endl;
}

void demo_macro_enums() {
    enumeration_begin(color) 
    declare_member(RED = 0) delimiter 
    declare_member(GREEN) delimiter 
    declare_member(BLUE) delimiter 
    declare_member(COUNTER)
    enumeration_end;
    for (int i = 0; i < color::COUNTER; i++) {
        switch (i)
        {
            case color::RED:
                std::cout << "red" << std::endl;
                break;
            case color::GREEN:
                std::cout << "green" << std::endl;
                break;
            case color::BLUE:
                std::cout << "blue" << std::endl;
                break;
            default:
                break;
        }
    }

    // Создание строк
    #undef enumeration_begin 
    #undef declare_member
    
    #define list(arg) arg##_strs
    #define enumeration_begin(arg) const char* const list(arg)[]={ 
    #define declare_member(arg) #arg 
    #define SizeOf(arg) sizeof arg##_strs / sizeof *arg##_strs

    enumeration_begin(colors)
    declare_member("Red") delimiter
    declare_member("Green") delimiter
    declare_member("Blue") delimiter
    enumeration_end;

    for (int i = 0; i < SizeOf(colors); i++) {
        std::cout << "Color: " << list(colors)[i] << std::endl;
    }
}

int main(int argc, char* argv[])
{
    assert("Macro test");
    TryMacros  test(12,"hello");
    cout << test.getNumber() << " " << test.getString() << endl;
    int __macro_join(test,var) = 15;
    cout << "Macro join = " << testvar << endl;
    cout << "Hello, "__macro_make_string(World!) << endl;
    // Стандартные макросы
    cout << "Processed on " << __LINE__ << " line" << endl;
    cout << "Source file name is " << __FILE__ << endl;
    cout << "COUNTER IS " << __COUNTER__ << endl;
    cout << "Preprocessor launched in: " __DATE__ << endl
                                     <<  __TIME__ << endl
                                     <<  __TIMESTAMP__ << endl;
    
    /* __STDC__ заменяется на 1, если компиляция происходит в соответствии со стандартом языка С
     * 
     */                                 
    cout << "Compile with standard: " << __STDC__ << " or " << __STDC_VERSION__ << endl
         << "Program has been controlling by: " << __STDC_HOSTED__ << endl;
    cout << "System support:" << endl
         << "-- operations with floating numbers = " << __STDC_IEC_559__ << endl
         << "-- comlex numbers = " << __STDC_IEC_559_COMPLEX__ << " and " << __STDC_NO_COMPLEX__ << endl
         << "-- masses with variable length = " << __STDC_NO_VLA__ << endl
         << "-- macros' with variable args = " << __VA_ARGS__ << endl
         ;
    
    function();
    demo_macro_enums();
    return 0;
}
