/**
 * @file colon.cpp
 * @author Grigory Okhmak (ohmak88@yandex.ru)
 * @details
 * На практике такая ситуация встречается не очень часто, но следует знать следующий синтаксис,
 * чтобы не теряться в чужом коде. Иногда возникают ситуации, когда имена в вашем пространстве
 * имен пересекаются с именами в глобальном пространстве, или высока вероятность такого пересечения.
 * В этом случае, если вы хотите гарантировать вызов из глобального пространства имен, вы должны
 * поставить два двоеточия напротив вызова функции или объекта из глобального пространства имен.
 * @version 0.1
 * @date 2019-04-13
 * 
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 */

#include <iostream>

class GlobalClass {
    public:
        static void say() { std::cout << "global space: method" << std::endl; }
};

void say() { std::cout << "global space: function" << std::endl; }

char cLine[] = "Global line";

//================================================

namespace SomeSpace {

char cLine[] = "Space line";

class GlobalClass {
    public:
        static void say() { std::cout << "some space: method" << std::endl; }
};

void say() { std::cout << "some space: function" << std::endl; }

void client() {
    /*
     * Допустим клиент хочет обратиться к объектам в области видимости в которой он
     * сейчас находится. Ничего сложного в этом нет.
     */
    std::cout << cLine << std::endl;
    GlobalClass::say();
    say();
    
    /*
     * Но что делать когда в глобальной видимости есть такие же имена? Для обращения
     * к ним следует использовать следующий синтаксис.
     */
    std::cout << ::cLine << std::endl;
    ::GlobalClass::say();
    ::say();
}

}

int main(int argc, char* argv[]) 
{
    SomeSpace::client();
    return 0;
}