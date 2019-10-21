#include <string>
#include <iostream>

class A {
    public:
        static A& instance() {
            static A inst;
            return inst;
        }
        std::string hello() { return "hello"; }
};

A& (&manage)() = A::instance;
using inst = A& (&)();

inst manager = A::instance;

template<typename C>
using tinst = C& (&)();

int main(int argc, char* argv[])
{
    std::cout << manage().hello() << std::endl;
    std::cout << manager().hello() << std::endl;

    tinst<A> mng = A::instance;
    std::cout << mng().hello() << std::endl;

    return 0;
}
