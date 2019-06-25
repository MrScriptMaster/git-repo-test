/**
 * @file singleton.cpp
 * @author Grigory Okhmak (ohmak88@yandex.ru)
 * @details 
 * В этом файле приведено одно из решений реализации паттерна "одиночка".
 * Для создания однотипных одиночек используется шаблонный класс.
 * @version 0.1
 * @date 2019-02-05
 * 
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 */

//#pragma once

#include <cstddef>          // для NULL
#include <iostream>

template <typename SingletonClassName>
class CSingleton
{
protected:
	CSingleton() { }
	static SingletonClassName* m_pSelf;
public:

	static SingletonClassName& Instance()
	{
		if (m_pSelf == NULL)
		{
			m_pSelf = new SingletonClassName();
		}
		return *m_pSelf;
	}

	static void Destroy()
	{
		if (m_pSelf != NULL)
		{
			delete m_pSelf;
			m_pSelf = NULL;
		}
	}

	static bool IsDestroyed()
	{
		return (m_pSelf == NULL);
	}
};

template<typename SingletonClassName>
SingletonClassName* CSingleton<SingletonClassName>::m_pSelf = NULL;

// --- cut off ---------------------------------------------------- cut off ---

/*
 * Теперь для создания одиночек достаточно наследовать шаблонный класс, где в качестве
 * шаблонного параметра следует передавать имя класса. Преимущество от такого решения в
 * основном в однотипности реализации паттерна для всего проекта.
 */

class ClassA : public CSingleton<ClassA> {
    public:
        friend class CSingleton<ClassA>;
        void SayHello()
        {
            std::cout << "Class A says hello" << std::endl;
        }
    
    protected:
        ClassA()                           = default;
        virtual ~ClassA()                  = default;
        ClassA(ClassA const &)             = default;
        ClassA& operator= (ClassA const &) = default;
};

class ClassB : public CSingleton<ClassB> {
    public:
        friend class CSingleton<ClassB>;
        void SayGoodBye()
        {
            std::cout << "Class B says good bye" << std::endl;
        }
    
    protected:
        ClassB()                           = default;
        virtual ~ClassB()                  = default;
        ClassB(ClassB const &)             = default;
        ClassB& operator= (ClassB const &) = default;
};

int main(int argc, char* argv[]) 
{
    ClassA::Instance().SayHello();
    ClassB::Instance().SayGoodBye();
    return 0;
}

/*
 * БУДЬТЕ ОСТОРОЖНЫ !!!
 * 
 * При неправильном использовании этого решения возможны утечки памяти. Обратите внимание, что удаление
 * одиночек производится вручную методом Destroy(), так как у шаблона нет явного деструктора. Это означает, что вы должны
 * четко понимать сроки жизни одиночки.
 * 
 * Это решение ориентировано на создание одиночек, которые существуют все время работы программы. Однако,
 * вы можете доработать класс, добавив явный деструктор, который удаляет одиночку автоматически.
 */