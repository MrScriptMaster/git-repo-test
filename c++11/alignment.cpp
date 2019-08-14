/**
 * @file alignment.cpp
 * @author Grigory Okhmak (ohmak88@yandex.ru)
 * @details
 * Чтение и запись данных происходит в любой программе, однако когда доходит время до хранения
 * данных на носителе, возникают проблемы с переносимостью. Дело в том, что разные архитектуры
 * представляют структуры данных по-разному: порядок записи байтов может быть прямой или обратный;
 * к структурам может применяться или не применяться выравнивание. Также разные компьютеры
 * передают данные так, как они "привыкли" с ними работать, поэтому одни и те же данные на
 * разных компьютерах могут по разному интерпретироваться именно из-за того, что разработчики
 * не подумали о переносимости.
 * 
 * Обычно, когда структуры хрянятся в опреативной памяти, предпочтительно обращаться к ним, когда
 * они выравнены в памяти по машинным словам, так как время доступа к ним сокращается, если
 * конечно это вообще не единственный способ доступа к ним. Однако, когда данные нужно куда-то
 * сохранить, то такое выравнивание может тратить много памяти из-за промежутков между полями,
 * поэтому обычно прибегают к уплотнению.
 * 
 * Процедура, после которой структура записывается одним фрагментом без промежутков, называется
 * сериализацией структуры. Обратная процедура называется десериализацией, причем эта процедура
 * не обязательно должна завершаться выравниванием.
 * 
 * В этом файле мы рассмотрим некоторые приемы, связанные с выравниванием.
 * @version 0.1
 * @date 2019-08-14
 * 
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 */

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <cstring>

struct data_test_1 {
    unsigned int    m_integer;
    char            m_char1;
    char            m_char2;
    float           m_float;
    double          m_double;
    data_test_1() 
    : m_integer(-1)
    ,m_char1('a')
    ,m_char2('b')
    ,m_float(3.14)
    ,m_double(-5e25)
    {}
};

/*
 * Если посчитать размер этой структуры с ходу, то для 32-х разрядной системы
 * она должна занимать 8 байт. Однако, это на самом деле не обязательно и зависит от того,
 * будет ли структура выравнена по машинномму слову. Если структура выравнивается по
 * кратным машинному слову адресам, то она будет занимать 12 байт.
 */
struct data_test_2 {
    char    m_char;         // 1 байт
    short   m_short;        // 2 байта
    int     m_integer;      // 4 байта
    char    m_end;          // 1 байт
    data_test_2()
    {
        memset(this, 0, sizeof(data_test_2));
        m_char = 'a';
        m_short = 123;
        m_integer = 456;
    }
    void print()
    {
        short columnSize = 3;
        short i = 0;
        char* f1 = &(((data_test_2*)this)->m_char);
        printf("data_test_2 dump (sizeof=%d):\n", sizeof(data_test_2));
        printf("%08X:   ", f1);
        for (;f1 <= (char*)&(((data_test_2*)this)->m_end); f1++)
        {
            if (i > columnSize) {
                printf("\n%08X:   ", f1);
                i = 0;
            }
            printf("%08X ", *f1);
            i++;
        }
        printf("\n");
    }
};

struct data_test_3 {
    char    m_char;
    short   m_short1;
    short   m_short2;
    int     m_integer;
    char    m_end;
    data_test_3()
    {
        memset(this, 0, sizeof(data_test_3));
        m_char = 'a';
        m_short1 = 123;
        m_integer = 456;
    }
    void print()
    {
        short columnSize = 3;
        short i = 0;
        char* f1 = &(((data_test_3*)this)->m_char);
        printf("data_test_3 dump (sizeof=%d):\n",sizeof(data_test_3));
        printf("%08X:   ", f1);
        for (;f1 <= (char*)&(((data_test_3*)this)->m_end); f1++)
        {
            if (i > columnSize) {
                printf("\n%08X:   ", f1);
                i = 0;
            }
            printf("%08X ", *f1);
            i++;
        }
        printf("\n");
    }
};

#pragma pack(push,1)
struct data_test_4 {
    char    m_char;
    short   m_short;
    int     m_integer;
    char    m_end;
    data_test_4()
    {
        memset(this, 0, sizeof(data_test_4));
        m_char = 'a';
        m_short = 123;
        m_integer = 456;
    }
    void print()
    {
        short columnSize = 3;
        short i = 0;
        char* f1 = &(((data_test_4*)this)->m_char);
        printf("data_test_4 dump (sizeof=%d):\n", sizeof(data_test_4));
        printf("%08X:   ", f1);
        for (;f1 <= (char*)&(((data_test_4*)this)->m_end); f1++)
        {
            if (i > columnSize) {
                printf("\n%08X:   ", f1);
                i = 0;
            }
            printf("%08X ", *f1);
            i++;
        }
        printf("\n");
    }
};

struct data_test_5 {
    char    m_char;
    short   m_short1;
    short   m_short2;
    int     m_integer;
    char    m_end;
    data_test_5()
    {
        memset(this, 0, sizeof(data_test_5));
        m_char = 'a';
        m_short1 = 123;
        m_integer = 456;
    }
    void print()
    {
        short columnSize = 3;
        short i = 0;
        char* f1 = &(((data_test_5*)this)->m_char);
        printf("data_test_5 dump (sizeof=%d):\n",sizeof(data_test_5));
        printf("%08X:   ", f1);
        for (;f1 <= (char*)&(((data_test_5*)this)->m_end); f1++)
        {
            if (i > columnSize) {
                printf("\n%08X:   ", f1);
                i = 0;
            }
            printf("%08X ", *f1);
            i++;
        }
        printf("\n");
    }
};
#pragma pack(pop)

/*
 * Проверить то, какой порядок байтов использует система, можно посмотрев
 * на первый байт предопределенной константы, равной 1. Если он равен нулю, значит
 * единичный бит находится в младшей тетраде, т.е. порядок байт дальний.
 */
const int iOne = 1;
#define is_bigendian() ( (*(char*)&iOne) == 0 )
void test_big_little_endian(void) {
    long val;
    char *ptr;
    ptr = (char*) &val;
    val = 0x12345678;
    if (is_bigendian()) {
        std::cout << "System uses big-endian:" << std::endl;
        printf( "%X.%X.%X.%X\n", ptr[0], ptr[1], ptr[2], ptr[3]);
    } else {
        std::cout << "System uses little-endian:" << std::endl;
        printf( "%X.%X.%X.%X\n", ptr[3], ptr[2], ptr[1], ptr[0]);
    }
    std::cout << "You should see: 12.34.56.78" << std::endl;
}

/**
 * @brief Следующую функцию можно использовать для получения знания о порядке
 * байт.
 * @return true, если порядок байт дальний.
 */
bool is_big_endian()
{
    int i = 1;
    char* ptr = (char*)&i;
    return (ptr[0] == 0);
}

/*
 * Примеры работы функций по изменению порядка байт
 */
// Для типа short
short reverse_short(short sNum)
{
    unsigned char ch_1, ch_2;
    if (is_big_endian()) {
        return sNum;
    }
    else {
        ch_1 = sNum & 255;
        ch_2 = (sNum >> 8) & 255;
    }
    return (ch_1 << 8) + ch_2;
}
short reverse_short(char* chNum)
{
    short sNum;
    char* ptr = (char*)&sNum;
    if (is_big_endian()) {
        ptr[0] = chNum[0];
        ptr[1] = chNum[1];
    }
    else {
        ptr[0] = chNum[1];
        ptr[1] = chNum[0];
    }
    return sNum;
}
// Для типа int
int reverse_int(int iNum)
{
    unsigned char c1, c2, c3, c4;
    if (is_bigendian()) {
        return iNum;
    }
    else {
        c1 = iNum & 255;
        c2 = (iNum >> 8) & 255;
        c3 = (iNum >> 16) & 255;
        c4 = (iNum >> 24) & 255;
    }
    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}

void test_reverse_nums()
{
    std::cout << "Revese short:" << std::endl;
    //0000 0000 0111 1011‬                     = 123
    //0111 1011 0000 0000‬                     = 31 488
    //0111 1011 0000 0000 0000 0000 0000 0000 = 2 063 597 568
    short sNumber = 123; char* sPtr = (char*)&sNumber;
    std::cout << "Test 1: " << reverse_short(sNumber) << std::endl
    << "Test 2: " << reverse_short(sPtr) << std::endl;
    std::cout << "Revese int:" << std::endl;
    int iNumber = 123;
    std::cout << "Test 1: " << reverse_int(iNumber) << std::endl;
}

int main(int argc, char* argv[])
{
    //1
    test_big_little_endian();
    if (is_big_endian())
        std::cout << "Big endian" << std::endl;
    else
        std::cout << "Little endian" << std::endl;
    test_reverse_nums();
    //2
    std::cout << "Without packing:" << std::endl;
    data_test_2 test_1;
    test_1.print();
    data_test_3 test_2;
    test_2.print();
    std::cout << "With packing:" << std::endl;
    data_test_4 test_4;
    test_4.print();
    data_test_5 test_5;
    test_5.print();
    return 0;
}