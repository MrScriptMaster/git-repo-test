/**
 * @file templates.cpp
 * @author Grigory Okhmak (ohmak88@yandex.ru)
 * @details
 * Шаблоны в языке С++ позволяют генерировать код на этапе компиляции. Техника использования
 * особенностей обработки шаблонов на этапе компиляции называется метапрограммированием шаблонов
 * (англ. Template metaprogramming, TMP).
 * 
 * Известно следующее практическое использование TMP:
 *    - При разработке каркаса приложения, задавая тем самым платформу понятий и идиом. Например, на таком\n
 *      принципе строятся библиотеки-заголовки, хранящие весь свой код в одном или нескольких заголовочных файлах, обычно с расширением *.hpp.
 *    - Когда вы хотите использовать механизмы выведения типов во время компиляции - так называемые шаблоны-характеристики.
 * 
 * Метапрграммирование позволяет:
 *    - Уменьшить дублирование кода.
 *    - Увеличение портируемости кода.
 *    - Переложить на компилятор некоторые вычисления, которые можно выполнить до запуска программы, улучшая общую производительность программы.
 * 
 * Среди недостатков метапрограммирования отмечают:
 *    - Чем больше и сложнее метапрограмма, тем дольше работает компилятор.
 *    - Сложность чтения кода из-за особенностей синтаксиса шаблонов в С++.
 *    - Длительный процесс разработки шаблонов, потому что как правило переписывание их в дальнейшем\n
 *      в написанном приложении очень затруднительно. Т.е. фактически их нужно написать правильно сразу.
 *    - Никакого инструментария для поддержки метапрограмм по сути не существует. В настоящее время\n 
 *      используются общеизвестные договоренности, которые облегчает понимание метапрограмм.
 * 
 * ## Основы
 * ### Шаблоны-характеристики
 * Данные, которыми манипулирует компилятор, называются "метаданными". Метаданные принято разделять на "типы" и "не типы".
 * Метатипы применяются так же как и обычные типы, но только в метапрограмме. Текущая практика рекомендует, чтобы шаблоны-типы
 * имели имена, оканчивающиеся на *_traits. Такие шаблоны также называют "шаблоны-характеристики".
 * 
 * Типичный шаблон-характеристика строится так
 * @code
 * namespace Traits {
 *      struct type1 {...};
 *      struct type2 {...};
 *      //... больше тэгов
 * 
 *      template<>
 *      struct type1_traits<type1> {
 *          typedef int value_type;
 *          //... больше характеристик для этого типа
 *      };
 *      template<>
 *      struct type_2_traits<type2> {
 *          typedef int value_type;
 *          //... больше характеристик для этого типа
 *      };
 *      // и так далее
 * }
 * @endcode
 * Шаблоны характеристики призваны сообщать метафункциям характеристики приходящим через шаблонные определения типов,
 * для того чтобы метафункции приняли одну из реализаций кода. Например, в библиотеке STL такие шаблоны созданы
 * для каждой категории итераторов, чтобы сообщить метафункциям, которым итераторы могут передаваться по шаблону, какие
 * операции над этими итраторами доступны.
 * 
 * Приведенный выше пример не полный, так как он не учитывает возможность передачи указателей через шаблоны для этих типов. Так как по
 * указателю нельзя получать характеристики, шаблоны для них должны быть частично специфицированы.
 * @code
 * namespace Traits {
 *      //...
 *      template<typename T>
 *      struct some_type_traits<T*> {
 *          typedef T value_type;
 *          //... больше характеристик для этого типа
 *      };
 *      //...
 * }
 * @endcode
 * Из этих примеров вы должны вынести следующее:
 *      - Для шаблоны-характеристики следует объявлять исключительно как структуры, так как это облегчает метапрограммирование.
 *      - Шаблоны-характеристики следует оканчивать на *_traits, так как это общепринятая конвенция.
 * 
 * ### Метафункции
 * Метафункции - это такие функции, которые строят реализацию на этапе компиляции. Впинципе метафункции могут и не оперировать
 * метаданными. Метапрограммирование так или иначе сводится к идиомам, описанным ниже.
 * 
 * ### Вычисляющие шаблоны
 * Рассмотрим простой пример. Следующий шаблон вычисляет факториал
 * @code
 * template<unsigned n>
 * struct Factorial {
 *      enum { value = n * Factorial<n-1>::value };
 * };
 * template<>
 * struct Factorial<0> {
 *      enum { value = 1 };
 * };
 * @endcode
 * В этом примере показана основа любой вычисляющей метаконструкции - рекурсия. Так как рекурсия всегда должна быть конечна, то
 * разработчик всегда должен предусмотреть стоповый шаблон.
 * 
 * Для вычисляющих конструкций не обязательно использовать только struct: вы можете использовать шаблонные классы или шаблонные
 * функции, главное правильно запрограммировать рекурсию.
 * Любой вычисляющий шаблон следует следующей идиоме
 * @code
 * template<int i>
 * class Loop {
 *      public:
 *          static inline void do_something() {
 *              //... делаем что-нибудь
 *              Loop<i-1>::do_something();
 *              //... делаем что-нибудь
 *          }
 * };
 * template<>
 * class Loop<0> {
 *      public:
 *          static inline void do_something() {
 *              //... делаем что-нибудь
 *          }
 * };
 * @endcode
 * Полезными также являются идиомы If и Switch.
 * @code
 * // If
 * template<bool condition>
 * class If {
 *      public:
 *          static inline void action() { ... }
 * };
 * template<>
 * class If<false> {
 *      public:
 *          static inline void action() { ... }
 * };
 * 
 * // Switch
 * template<int variant>
 * class Switch {
 *      public:
 *          static inline void action() { ... }
 * };
 * template<>
 * class Switch<1> {
 *      public:
 *          static inline void action() { ... }
 * };
 * template<>
 * class Switch<2> {
 *      public:
 *          static inline void action() { ... }
 * };
 * // и так далее
 * @endcode
 * 
 * 
 * @version 0.1
 * @date 2019-02-24
 * 
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 * 
 */

#include <iostream>
#include <string>
#include <typeinfo>

using namespace std;

namespace ex1 {
    struct type1_tag {};
    struct type2_tag {};
    struct type11_tag : public type1_tag {};
    
    template <typename MyType>
    struct mtype_traits {
        typedef typename MyType::category category;
    };

    template<typename MyType>
    struct mtype_traits<MyType*> {
        typedef type11_tag category;
    };
    

    class One {
    public:
        typedef type1_tag category;
    };

    class Two {
    public:
        typedef type2_tag category;
    };

    template<typename MyType>
    void process(MyType& object, type1_tag)
    {
        std::cout << "Process 1" << std::endl;
    }

    template<typename MyType>
    void process(MyType& object, type2_tag)
    {
        std::cout << "Process 2" << std::endl;
    }

    template<typename MyType>
    void process(MyType& object, type11_tag)
    {
        std::cout << "Process 3" << std::endl;
    }

    template<typename MyType>
    void meta_function(MyType& object)
    {
        process(object, typename mtype_traits<MyType>::category());
    }
}

int main(int argc, char* argv[]) {
  
    ex1::One o1;
    ex1::Two o2;
    ex1::meta_function<ex1::One>(o1);
    ex1::meta_function(o1);
    ex1::meta_function(o2);
    return 0;
} 