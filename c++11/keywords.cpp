/**
 * @file keywords.cpp
 * @author Grigory Okhmak (ohmak88@yandex.ru)
 * @details
 * Начиная со стандарта С++11 появился ряд служебных слов и конструкций, позволяющих упростить программирование.
 * Здесь рассмотрены следующие ключевые слова и конструкции:
 *  - auto - позволяет доверить компилятору автоматически выводить типы.
 *  - nullptr - совместимый с современным стандартом нулевой указатель.
 *  - override - указывает компилятору проверять переопределение виртуальных функций.
 *  - final - подобно ключевому слову в Java, позволяет запретить переопределение виртуальной функции в потомках.
 *  - функции begin() и end().
 *  - static_assert()
 *  - decltype()
 * @version 0.1
 * @date 2019-04-13
 * 
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 * 
 */

#include <vector>
#include <iostream>
#include <type_traits>

namespace {
    /*
     * Ключевое слово auto позволяет доверить компилятору автоматически выводить тип на основании
     * значения, которым инициализируется переменная. Это позволяет сократить код, не указывая длинные
     * имена типов, которые вдобавок могут быть загромождены именами пространств имен и т.п.
     * 
     * "auto" не является именем типа, поэтому нельзя перечислять определения через запятую.
     */
    auto int_value = 25;
    auto long_value = 25L;
    auto long_long_value = 255LL;
    auto float_value = 3.1415f;
    auto double_value = 3.1415;    
    auto pointer = nullptr;
    auto pointer_1 = &int_value;

    /*
     * auto очень удобно для выведения итераторов в foreach циклах.
     */
    void print_vector() {
        std::vector<int> v = {1,2,3,4,5};
        for (auto &element : v) {
            std::cout << element << " ";
        }
        std::cout << std::endl;
        for (auto it = v.begin(); it != v.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << std::endl;
    }
    /*
     * Если r-value является ссылка, то auto будет выводить тип, на который ссылается ссылка.
     */
    int vv = 25;
    int& vv_ref = vv;
    auto bb = vv_ref;  // в bb значение будет скопировано 
    
    //auto& ai = 25;   // непавильно
    auto&& ai = 25;    // правильно
    /*
     * auto нельзя использовать в параметрах обычной функции, однако допустимо использовать
     * auto для вывода возвращаемого из функции значения.
     */
    template<typename T>
    auto function_1(T a, T b)
    {
        return (a + b);
    }
    auto function_2(int a, int b)
    {
        return (a + b);
    }
    /*
     * auto можно использовать в параметрах лямбда-функций, начиная с С++14 стандарта.
     */
    auto lambda = [](auto a) {
        return a;
    };

    /*
     * Вы можете выводить типы из уже объявленных определений с помощью ключевого слова decltype.
     * Это в основном полезно в шаблонных функциях.
     */
    double pi = 3.1415;
    decltype(pi) double_value_2 = pi;    // тип будет выведен из определения переменной pi
    /*
     * Допустимо использовать decltype с auto, начиная с С++14 стандарта. В этом случае тип выводится
     * из r-value значения. Вообще это аналогично объявления с помощью одного auto с тем отличием, что если
     * r-value это ссылка, то l-value тоже будет ссылкой. 
     */
    decltype(auto) reference = vv_ref;   // для l-value будет выведена именно ссылка, а не просто один тип.

    /*
     * На этапе компиляции теперь можно проверять утверждения с помощью static_assert. В основном это полезно
     * при программировании шаблонных функций и с type_traits выражениями.
     * 
     * static_assert прерывает компиляцию, если условие не выполняется, в противном случае ничего не делает.
     */
    template<typename T>
    auto test_static_assert(T input) {
        static_assert(std::is_unsigned<T>::value, "Input value must be 'unsigned'");
        return input;
    }

    /*
     * Можно определять строго типизированные enum
     */
    enum class Options { One, Two, None };
}

namespace ex1 {
    /*
     * Начиная с C++11 стандарта появилась поддержка виртуальных функций с помощью новых
     * ключевых слов override и final.
     * 
     * Ключевое слово override заставляет компилятор проверять сигнатуру переопределяемой
     * функции в родительском классе. Это позволяет избежать случайных ошибок, которые могли
     * возникать из-за невнимательности программиста. Во всех новых программах следует всегда
     * применять это слово для переопределяемых функций.
     */
    class A {
    public:
        virtual void print(int a) { std::cout << "A" << std::endl; } 
    };
    class B : public A {
    public:
        /*
         * Этот класс запрещает переопределять эту функцию дальше в цепочке наследования
         * с помощью слова final
         */
        void print(int a) override final { std::cout << "B" << std::endl; } 
    };
    class C : public B {
    public:
        /*
         * Предок этого класса запретил переопределять эту функцию, поэтому
         * следующее выражение ошибочно
         */
        //void print(int a) override { std::cout << "C" << std::endl; } 
        /*
         * Но если бы переопределять функцию можно было бы, то вы не смогли бы
         * совершить типичную ошибку, показанную ниже.
         */
        //void print(short a) override { std::cout << "C" << std::endl; } 
    };
}

int main(int argc, char* argv[])
{
    std::cout << function_1(2,2) << std::endl; 
    std::cout << function_2(2,2) << std::endl;
    print_vector();
    std::cout << lambda("Hello") << std::endl;
    std::cout << double_value_2 << std::endl;
    std::cout << reference << std::endl;
    std::cout << test_static_assert(25U) << std::endl;
    //std::cout << test_static_assert(-25) << std::endl;  // Эта строка не будет скомпилирована, так как шаблонная функция требует unsigned параметр

    return 0; 
}