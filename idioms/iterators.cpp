/**
 * @file iterators.cpp
 * @author Grigory Okhmak (ohmak88@yandex.ru)
 * @details 
 * На практике часто приходится создавать свои контейнеры, которые как правило агрегируют
 * один из стандартных. Обычно реализация хранения прячется, поэтому необходимо предусмотреть
 * способы перебора элементов в контейнере.
 * 
 * В современном С++ рекомендуется использовать интерфейс итераторов, особенно, если ваш
 * контейнер часть большой программной системы. Предпосылки для этого следующие: возможность
 * написания циклов в стиле foreach, что делает код в целом чище; возможность интеграции вашего
 * контейнера в стандартную библиотеку; единообразие интерфейса с точки зрения стандартной библиотеки.
 * @version 0.1
 * @date 2019-08-07
 * 
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
 */
// ПРИМЕЧАНИЕ: компилируйте этот файл с флагом --std=c++17.
/*
 * Основы
 * 
 * В С++17 все итераторы разделены по определенным категориям. Ниже мы перечислим эти категории
 * по силе: более сильная категория наследует возможности более слабой категории.
 * 
 *  - Итераторы ввода - призваны для считывания логической цепочки данных. Поддерживают операции
 *    инкремента и разыменовывания. Данные после инкрементирования становятся недействительными,
 *    поэтому одну последовательность данных нельзя пройти этим итератором более одного раза.
 *  - Однонаправленные итераторы - это итераторы ввода за исключением того, что один и тот же диапазон
 *    можно пройти любое число раз.
 *  - Двунаправленные итераторы - это итераторы, который может проходить диапазон в любом направлении,
 *    любое число раз.
 *  - Итераторы с произвольным доступом - итераторы, способные проходить диапазон не по порядку, а перепрыгивать
 *    элементы в любом направлении.
 *  - Непрерывные итераторы - итераторы, которые требуют, чтобы данные хранились в непрерывной памяти.
 * 
 *  Кроме того, итераторы могут быть:
 *  - Итераторы вывода - данные итераторы нельзя читать, а служат они для того, чтобы указывать место,
 *    в которе нужно записать данные. Данные итераторы можно только инкрементировать.
 *  - Изменяемые итераторы - это сочетание итератора вывода с итератором из любой вышеперечисленной категории.
 * 
 * Ниже показана таблица операций, которые должны поддерживаться разными категориями итераторов. Более
 * сильная категория итераторов должна поддерживать операции более слабой.
 * 
 * ***************************************************************************************************
 *   Категория                              Операции
 * ***************************************************************************************************
 *   Все итераторы           - Конструктор копирования + оператор присваивания (=)
 *                           - Левосторонний и правосторонний инкремент (++)
 * 
 *   Оператор ввода          - Операторы равно и не равно ( == и != )
 *                           - Оператор разыменовывания как rvalue (*)
 *   
 *   Оператор вывода         Оператор разыменовывания как lvalue (*)
 * 
 *   Однонаправленный        Должен поддерживать многопроходность
 *   итератор
 * 
 *   Двунаправленный         Левосторонний и правосторонний декремент (--)
 *   итератор
 * 
 *   Итератор с              - Поддержка операторов сложения и вычитания с целым числом (+ и -)
 *   произвольным            - Поддержка прочих операций сравнения итераторов (< > <= >=)
 *   доступом                - Поддержка операторов += и -=
 *                           - Поддержка разыменовывания по индексу (оператор []).
 *
 * ***************************************************************************************************
 */

/*
 * Простейший итератор ввода
 */

//#pragma once
#include <iostream>
#include <iterator>
#include <initializer_list>
#include <algorithm>
#include <memory>
#include <string>
#include <climits>

/*
 * В следующем примере мы создадим простейший итератор. Простейший итератор должен поддерживать
 * минимум 4 оператора: * (разыменование), ++ (инкремент), == (логическое равно), != (не равно).
 */

class CNumIterator {
public:
    explicit CNumIterator(int pos = 0) : m_data(pos) {}

    int operator*() const { return m_data; }
    CNumIterator& operator++() {
        ++m_data;
        return *this;
    }
    bool operator!=(const CNumIterator& other) const {
        return m_data != other.m_data;
    }
    bool operator==(const CNumIterator& other) const {
        return !(*this != other);
    }
private:
    int m_data;
};

/* 
 * Предыдущего требования достаточно для не STL итератора, однако, если вы планируете интегировать
 * их в алгоритмы STL, то необходимо прикрепить STL интерфейс итераторов. Делается это через
 * шаблоны iterator_traits.
 */

namespace std {
    template<>
    struct iterator_traits<CNumIterator> {
        using iterator_category = std::forward_iterator_tag;
        using value_type        = int;
    };
    /* С помощью этого типажа мы указали характеристики нашего итератора, некоторые из которых
     * используются для выбора реализации алгоритма. Обратите внимание, что характеристики могут
     * добавляться. Также характеристики могут иметь разные возможные значения в зависимости
     * от стандарта. Поэтому всегда уточняйте возможные значения из документации к реализации
     * STL.
     * 
     * Типаж iterator_traits содержит до 5 характеристик:
     *  - difference_type - значение какого типа мы получим, когда будем вычитать один итератор из другого.
     *  - value_type - какой тип мы получаем в результате разыменовывания.
     *  - pointer - к какому типу должен относиться указатель, чтобы иметь возможность указывать на элемент.
     *  - reference - к какому типу должна относится ссылка.
     *  - iterator_category - категория итератора:
     *    -- input_iterator_tag - итератор ввода;
     *    -- output_iterator_tag - итератор вывода;
     *    -- forward_iterator_tag - однонаправленный итератор;
     *    -- bidirectional_iterator_tag - двунаправленный итератор;
     *    -- random_access_iterator_tag - итератор с произвольным доступом.
     * 
     * Характеристики pointer, reference и difference_type используются, когда итератор указывает на реальные
     * участки памяти.
     */
}

/*
 * Как правило, итераторы не существуют сами по себе, а порождаются некоторыми объектами,
 * например, контейнерами в стандартной библиотеке. Чтобы это было возможным, такой объект
 * должен иметь две функции члена: begin() и end().
 * 
 * Обратите внимание, что очень важно делать определенные методы для итератора константными, иначе
 * такой код может быть не принят компилятором.
 */

class CNumRange {
public:
    // Примечание: правая граница в диапазон не входит.
    CNumRange(int left, int right) : m_leftBorder(left), m_rightBorder(right) {}

    CNumIterator begin() const { return CNumIterator(m_leftBorder); }
    CNumIterator end() const { return CNumIterator(m_rightBorder); }
private:
    int m_leftBorder;
    int m_rightBorder;
};

/*
 * Ниже показан еще один подход к реализации итераторов. Обычно такой подход
 * удобен для контейнеров. Далее мы реализуем непрерывный итератор.
 * 
 * Чтобы не описывать характеристики, как мы сделали это выше, мы можем унаследовать
 * интерфейс итератора std::iterator, где характеристики передаются через параметры
 * шаблона.
 */
class CIntArray;

template<typename T>
class CIterator : public std::iterator<std::input_iterator_tag, T>
{
    friend class CIntArray;
public:
    CIterator(const CIterator& it) : m_ptr(it.m_ptr) {}

    bool operator!=(const CIterator& other) const {
        return m_ptr != other.m_ptr;
    }
    bool operator==(const CIterator& other) const {
        return m_ptr == other.m_ptr;
    }
    CIterator& operator++() {
        ++m_ptr;
        return *this;
    }
    typename CIterator::reference operator*() const {
        return *m_ptr;
    }
private:
    CIterator(T* ptr) : m_ptr(ptr) {}
    T* m_ptr;
};

/*
 * Наш непрерывный контейнер.
 */
class CIntArray {
public:
    typedef CIterator<int> iterator;
    typedef CIterator<const int> const_iterator;

    CIntArray(std::initializer_list<int> ivals)
    : m_arraySize(ivals.size())
    ,m_data(new int[m_arraySize])
    {
        std::copy(ivals.begin(), ivals.end(), m_data.get());
    }

    iterator begin() {
        return iterator(m_data.get());
    }
    const_iterator begin() const {
        return const_iterator(m_data.get());
    }
    iterator end() {
        return iterator(m_data.get() + m_arraySize);
    }
    const_iterator end() const {
        return const_iterator(m_data.get() + m_arraySize);
    }
private:
    const size_t m_arraySize;
    std::unique_ptr<int[]> m_data;
};

/*
 * Итераторы не обязательно использовать для прохода коллекций по порядку.
 * Сами итераторы могут задавать алгоритм прохода. Как правило, это реализуется
 * через оборачивание существующих итераторов.
 * 
 * Ниже показаны поучительные примеры использования итераторов.
 */
namespace SpecialIterator {
    /*
     * Сортирующий итератор
     * Итератор, который проходит коллекцию так, как если бы она была отсортирована.
     * При этом реальной сортировки не происходит, т.е. коллекция не изменяется
     * во время итерирования.
     * 
     * Автор: Bukov Anton (k06aaa@gmail.com)
     */
    enum eOrder {
        etASCENDING = 0   // сортировка по возрастанию
        ,etDESCENDING       // сортировка по убыванию
    };
      
    template<
        typename ItCategory,             /// категория итератора
        typename ItType,                 /// класс итератора  
        typename T,                      /// тип итерируемых данных    
        eOrder order = etASCENDING  >    /// порядок сортировки
    class sort_iterator : public std::iterator<ItCategory,T>
    {
    public:
        ItType m_begin;         /// итератор на первый элемент
        ItType m_end;           /// итератор на последний элемент
        ItType m_prevIter;      /// итератор на текущий элемент
        int    m_index;         /// внутренний индекс для элементов

        sort_iterator()
        : m_begin(ItType())
        ,m_end(ItType())
        ,m_prevIter(ItType())
        ,m_index(0)
        {}
        sort_iterator(const ItType& begin, const ItType& end)
        :m_begin(begin)
        ,m_end(end)
        ,m_prevIter(ItType())
        ,m_index(0)
        {
            if (m_begin == m_end)
            {
                m_prevIter = m_end;
                return;
            }
            if (order == etASCENDING)
                makeSortStepForward();
            else
                makeSortStepBackward();            
        }
    private:
        void makeSortStepForward()
        {
            // Если достигли конца контейнера
            if (m_index == m_end - m_begin)
            {
                m_prevIter = m_end;
                return;
            }
            // Когда еще не было ни одного прохода, ищем следующий элемент с начала
            if (m_index == 0)
            {
                m_prevIter = m_begin;
                for (ItType it = m_begin; it != m_end; ++it)
                    if (*it < *m_prevIter) m_prevIter = it;
            }
            // иначе ищем следующий элемент, опираясь на найденный ранее
            else
            {
                ItType minValueIter = m_begin;
                for (ItType it = m_begin; it != m_end; ++it)
                {
                    if (*m_prevIter < *it)
                    {
                        if (*it < *minValueIter)
                            minValueIter = it;
                    }
                    else if (*m_prevIter == *it)
                    {
                        if (*m_prevIter < *it)
                        {
                            minValueIter = it;
                            break;
                        }
                    }
                }
                m_prevIter = minValueIter;
            }
            m_index++;
        }

        void makeSortStepBackward()
        {
            // Если достигли конца контейнера
            if (m_index == m_end - m_begin)
            {
                m_prevIter = m_end;
                return;
            }
            // Когда еще не было ни одного прохода, ищем следующий элемент с начала
            if (m_index == 0)
            {
                m_prevIter = m_begin;
                for(ItType it = m_begin; it != m_end; ++it)
                    if (*m_prevIter < *it) m_prevIter = it;
            }
            // иначе ищем следующий элемент, опираясь на найденный ранее
            else
            {
                ItType maxValueIter = m_begin;
                for (ItType it = m_begin; it != m_end; ++it)
                {
                    if (*it < *m_prevIter)
                    {
                        if (*maxValueIter == *m_prevIter)
                            maxValueIter = it;
                        else if (*maxValueIter < *it)
                            maxValueIter = it; 
                    }
                    else if (*m_prevIter == *it)
                    {
                        if (*it < *m_prevIter)
                        {
                            maxValueIter = it;
                            break;
                        }
                    }
                }
                m_prevIter = maxValueIter;
            }
            m_index--;
        }

    public: /* Методы итератора */
        bool operator== (const sort_iterator& rhs) const { return (m_prevIter == rhs.m_prevIter); }
        bool operator== (const ItType& rhs) const { return (m_begin + m_index == rhs); }
        bool operator!= (const sort_iterator& rhs) const { return !(*this == rhs); }
        bool operator!= (const ItType& rhs) const { return !(*this == rhs); }
        
        typename std::iterator<ItCategory, T>::value_type& 
        operator* () { return *m_prevIter; }

        sort_iterator& operator++ ()
        {
            if (order == etASCENDING)
                makeSortStepForward();
            else
                makeSortStepBackward();
            return *this;
        }

        sort_iterator operator++ (int) 
        {
            sort_iterator tmp(*this);
            operator++();
            return tmp;
        }
        
        sort_iterator & operator-- ()
        {
            if (order == etASCENDING)
                makeSortStepBackward();
            else
                makeSortStepForward();
            return *this;
        }

        sort_iterator operator-- (int) 
        {
            sort_iterator tmp(*this);
            operator--();
            return tmp;
        }

        sort_iterator & operator += (int n)
        {
            for (int i = 0; i < n; i++)
                ++m_begin;
            return *this;
        }

        sort_iterator & operator -= (int n)
        {
            m_begin -= n;
            return *this;
        }

        bool operator < (const sort_iterator & other) const { return (m_begin < other.m_begin); }
        bool operator > (const sort_iterator & other) const { return (m_begin > other.m_begin); }
        bool operator <= (const sort_iterator & other) const { return (m_begin <= other.m_begin); }
        bool operator >= (const sort_iterator & other) const { return (m_begin >= other.m_begin); }

        sort_iterator operator [] (int n) const
        {
            sort_iterator tmp(*this);
            return tmp += n;
        }
    };
    // Вспомогательные операторы
    template<typename Category, typename IterType, typename T, eOrder order>
    sort_iterator<Category,IterType,T,order> operator + (
        const sort_iterator<Category,IterType,T,order> & der, int n)
    {
        sort_iterator<Category,IterType,T,order> tmp(der);
        return tmp += n;
    }

    template<typename Category, typename IterType, typename T, eOrder order>
    sort_iterator<Category,IterType,T,order> operator + (
        int n, const sort_iterator<Category,IterType,T,order> & der)
    {
        sort_iterator<Category,IterType,T,order> tmp(der);
        return tmp += n;
    }

    template<typename Category, typename IterType, typename T, eOrder order>
    sort_iterator<Category,IterType,T,order> operator - (
        const sort_iterator<Category,IterType,T,order> & der, int n)
    {
        sort_iterator<Category,IterType,T,order> tmp(der);
        return tmp -= n;
    }

    template<typename Category, typename IterType, typename T, eOrder order>
    ptrdiff_t operator - (
        const sort_iterator<Category,IterType,T,order> & a,
        const sort_iterator<Category,IterType,T,order> & b)
    {
        return (a.begin - b.begin);
    }

    template<typename Category, typename IterType, typename T, eOrder order>
    ptrdiff_t operator - (
        const sort_iterator<Category,IterType,T,order> & a,
        const IterType & b)
    {
        return (a.begin - b);
    }

    // Вспомогательные функции
    template<eOrder order, typename IterType>
    sort_iterator<typename std::iterator_traits<IterType>::iterator_category,
                  IterType,
                  typename std::iterator_traits<IterType>::value_type,
                  order>
    sorter(IterType begin, IterType end = IterType())
    {
        if (end == IterType())
            end = begin;
        return sort_iterator<typename std::iterator_traits<IterType>::iterator_category,
                             IterType,
                             typename std::iterator_traits<IterType>::value_type,
                             order>(begin,end);
    }

    template<typename IterType>
    sort_iterator<typename std::iterator_traits<IterType>::iterator_category,
                  IterType,
                  typename std::iterator_traits<IterType>::value_type,
                  etASCENDING>
    sorter(IterType begin, IterType end = IterType())
    {
        if (end == IterType())
            end = begin;
        return sort_iterator<typename std::iterator_traits<IterType>::iterator_category,
                             IterType,
                             typename std::iterator_traits<IterType>::value_type,
                             etASCENDING>(begin,end);
    }

    /*
     * Полевый итератор
     * Итератор, который может обойти коллекцию из пользовательских структур по конкретному полю. 
     * Итератор работает при условии, что поля коллекции лежат в памяти последовательно.
     * Реализация может быть зависима от размера указателей.
     * 
     * Автор: Bukov Anton (k06aaa@gmail.com)
     */

    /**
     * @brief Вспомогательный макрос. Выделяет поле структуры, по которому требуется обход,
     * через вычисление смещения от ее начала.
     * 
     * @param Object Имя структуры.
     * @param field Поле структуры, по которому требуется обход.
     */
    #define fieldof(Object,field) (&(((Object*)NULL)->field))
    
    template<typename Category,   /// категория итератора
             typename IterType,   /// тип итератора
             typename FieldType>  /// тип хранимых в поле данных
    class field_walk_iterator : public std::iterator<Category, FieldType>
    {
    public:
        IterType it;
        FieldType* field;
        
        field_walk_iterator(FieldType* field)
            : it(IterType()), field(field)
        {}
        field_walk_iterator(IterType& it, FieldType* field)
            : it(it), field(field)
        {}

        bool operator != (const field_walk_iterator & rhs) const { return !(*this == rhs); }
        bool operator == (const field_walk_iterator & rhs) const { return (it == rhs.it);  }

        field_walk_iterator & operator ++ ()
        {
            ++it;
            return *this;
        }

        field_walk_iterator operator ++ (int) 
        {
            field_walk_iterator tmp(*this);
            operator++();
            return tmp;
        }

        FieldType operator * () const
        {
            return *(FieldType&)((intptr_t)field) + ((intptr_t)&(*it));
        }

        FieldType & operator * ()
        {
            intptr_t a = (intptr_t)&(*it);
            intptr_t b = (intptr_t)field;
            return *(FieldType*)(a + b);
        }

        field_walk_iterator & operator -- ()
        {
            --it;
            return *this;
        }

        field_walk_iterator operator -- (int) 
        {
            field_walk_iterator tmp(*this);
            operator--();
            return tmp;
        }

        field_walk_iterator & operator += (int n)
        {
            it += n;
            return *this;
        }

        field_walk_iterator & operator -= (int n)
        {
            it -= n;
            return *this;
        }

        bool operator < (const field_walk_iterator& der) const
        {
            return (it < der.it);
        }

        bool operator > (const field_walk_iterator& der) const
        {
            return (it > der.it);
        }

        bool operator <= (const field_walk_iterator& der) const
        {
            return (it <= der.it);
        }

        bool operator >= (const field_walk_iterator& der) const
        {
            return (it >= der.it);
        }

        field_walk_iterator operator [] (int n) const
        {
            field_walk_iterator tmp(*this);
            return tmp += n;
        }
    };

    // Вспомогательные операторы
    template<typename Category, typename IterType, typename T>
    field_walk_iterator<Category,IterType,T> operator + (
        const field_walk_iterator<Category,IterType,T>& der, int n)
    {
        field_walk_iterator<Category,IterType,T> tmp(der);
        return tmp += n;
    }

    template<typename Category, typename IterType, typename T>
    field_walk_iterator<Category,IterType,T> operator + (
        int n, const field_walk_iterator<Category,IterType,T>& der)
    {
        field_walk_iterator<Category,IterType,T> tmp(der);
        return tmp += n;
    }

    template<typename Category, typename IterType, typename T>
    field_walk_iterator<Category,IterType,T> operator - (
        const field_walk_iterator<Category,IterType,T>& der, int n)
    {
        field_walk_iterator<Category,IterType,T> tmp(der);
        return tmp -= n;
    }

    template<typename Category, typename IterType, typename T>
    ptrdiff_t operator - (
        const field_walk_iterator<Category,IterType,T>& a,
        const field_walk_iterator<Category,IterType,T>& b)
    {
        return (a.it - b.it);
    }

    template<typename Category, typename IterType, typename T>
    ptrdiff_t operator - (
        const field_walk_iterator<Category,IterType,T>& a,
        const IterType & b)
    {
        return (a.it - b);
    }

    // Вспомогательные функции
    template<typename IterType, typename T>
    field_walk_iterator<typename std::iterator_traits<IterType>::iterator_category,IterType,T>
    field_walker(IterType it, T * field)
    {
        return field_walk_iterator<typename std::iterator_traits<IterType>::iterator_category,IterType,T>(it,field);
    }

    /*
     * Битовый итератор
     * Позволяет проходить по битам данных.
     * 
     * Автор: Bukov Anton (k06aaa@gmail.com)
     */
    template<typename Category,     /// категория итератора
             typename IterType,     /// тип итератора
             typename T = int>      /// 
    class bit_walk_iterator : public std::iterator<Category, T>
    {
    public:
        IterType it;
        int nextBit;

    public:
        bit_walk_iterator()
            : it(), nextBit(int())
        {}

        bit_walk_iterator(IterType & it)
            : it(it), nextBit(int())
        {}

        bit_walk_iterator & operator ++ ()
        {
            nextBit++;
            if (nextBit == CHAR_BIT*sizeof(typename std::iterator<Category, T>::value_type))
            {
                ++it;
                nextBit = 0;
            }
            return *this;
        }

        bit_walk_iterator operator ++ (int) 
        {
            bit_walk_iterator tmp(*this);
            operator++();
            return tmp;
        }

        bool operator == (const bit_walk_iterator & rhs) const
        {
            return (it == rhs.it) && (nextBit == rhs.nextBit);
        }

        bool operator != (const bit_walk_iterator & rhs) const
        {
            return !(*this == rhs);
        }

        const T value () const
        {
            int byteNumber = nextBit / 8;
            int bitNumber  = nextBit % 8;
            char & ch = byteNumber[(char*)&(*it)];
            if ((*it) & (1 << bitNumber))
                return 1;
            return 0;
        }

        const T operator * () const
        {
            return value;
        }

        bit_walk_iterator & operator * ()
        {
            return *this;
        }

        template<typename TParam>
        bit_walk_iterator & operator = (TParam x)
        {
            int byteNumber = nextBit / 8;
            int bitNumber  = nextBit % 8;
            char & ch = byteNumber[(char*)&(*it)];
            ch &= (-1)^(1 << bitNumber);
            ch |= ((x?1:0) << bitNumber);
            return *this;
        }

        operator T () const
        {
            return value();
        }

        bit_walk_iterator & operator -- ()
        {
            nextBit--;
            if (nextBit == -1)
            {
                --it;
                nextBit = CHAR_BIT*sizeof(typename std::iterator<Category, T>::value_type)-1;
            }
            return *this;
        }

        bit_walk_iterator operator -- (int) 
        {
            bit_walk_iterator tmp(*this);
            operator--();
            return tmp;
        }

    private:
        bit_walk_iterator & move()
        {
            int bitsInObject = CHAR_BIT*sizeof(typename std::iterator<Category, T>::value_type);

            if (nextBit < 0)
            {
                it -= ((-nextBit) / bitsInObject)
                      + ((-nextBit) % bitsInObject > 0);
                nextBit = bitsInObject - ((-nextBit) % bitsInObject);
            } else
            if (nextBit > bitsInObject)
            {
                it += nextBit / bitsInObject;
                nextBit %= bitsInObject;
            }
        
            return *this;
        }

    public:
        bit_walk_iterator & operator += (int n)
        {
            nextBit += n;
            return move();
        }

        bit_walk_iterator & operator -= (int n)
        {
            nextBit -= n;
            return move();
        }

        bool operator < (const bit_walk_iterator & der) const
        {
            return (it < der.it);
        }

        bool operator > (const bit_walk_iterator & der) const
        {
            return (it > der.it);
        }

        bool operator <= (const bit_walk_iterator & der) const
        {
            return (it <= der.it);
        }

        bool operator >= (const bit_walk_iterator & der) const
        {
            return (it >= der.it);
        }

        bit_walk_iterator operator [] (int n) const
        {
            bit_walk_iterator tmp(*this);
            return tmp += n;
        }
    };

    // Вспомогательные операторы
    template<typename Category, typename IterType, typename T>
    bit_walk_iterator<Category,IterType,T> operator + (
        const bit_walk_iterator<Category,IterType,T> & der, int n)
    {
        bit_walk_iterator<Category,IterType,T> tmp(der);
        return tmp += n;
    }

    template<typename Category, typename IterType, typename T>
    bit_walk_iterator<Category,IterType,T> operator + (
        int n, const bit_walk_iterator<Category,IterType,T> & der)
    {
        bit_walk_iterator<Category,IterType,T> tmp(der);
        return tmp += n;
    }

    template<typename Category, typename IterType, typename T>
    bit_walk_iterator<Category,IterType,T> operator - (
        const bit_walk_iterator<Category,IterType,T> & der, int n)
    {
        bit_walk_iterator<Category,IterType,T> tmp(der);
        return tmp -= n;
    }

    template<typename Category, typename IterType, typename T>
    ptrdiff_t operator - (
        const bit_walk_iterator<Category,IterType,T> & a,
        const bit_walk_iterator<Category,IterType,T> & b)
    {
        return (a.it - b.it) + (a.nextBit - b.nextBit);
    }

    template<typename Category, typename IterType, typename T>
    ptrdiff_t operator - (
        const bit_walk_iterator<Category,IterType,T> & a,
        const IterType & b)
    {
        return (a.it - b.it)*CHAR_BIT*sizeof(typename std::iterator<Category, T>::value_type)
             + (CHAR_BIT*sizeof(typename std::iterator<Category, T>::value_type) - a.nextBit);
    }

    // Вспомогательные функции
    template<typename T, typename IterType>
    bit_walk_iterator<typename std::iterator_traits<IterType>::iterator_category,IterType,T>
    bit_walker(IterType it)
    {
        return it;
    }

    template<typename IterType>
    bit_walk_iterator<typename std::iterator_traits<IterType>::iterator_category,IterType,int>
    bit_walker(IterType it)
    {
        return it;
    }
}

// --- cut off ---------------------------------------------------- cut off ---

using namespace std;

int main(int argc, char* argv[])
{
    //1
    /*
     * Теперь мы можем итерировать наш объект циклом в стиле foreach.
     */
    for (auto num : CNumRange(1,10))
    {
        cout << num << " ";
    }
    cout << endl;

    //2
    /*
     * Так как к нашему итератору прикреплен STL интерфейс, то мы можем
     * применять к нему алгоритмы STL.
     */
    CNumRange range(100, 110);
    auto [minIt, maxIt] (minmax_element(begin(range), end(range)));
    cout << *minIt << " - " << *maxIt << endl;

    //3
    for (auto& elem : CIntArray({1,2,3,4,5,6,7,8,9,10}))
    {
        cout << elem << " ";
    }
    cout << endl;

    //4
    int input[]  = { 9,4,6,5,2,1 };
    size_t size = sizeof input / sizeof *input;
    {
        auto it = SpecialIterator::sorter<SpecialIterator::etASCENDING>(input, input + size + 1);
        cout << "Test array [size=" << size << "]" << endl;
        for (; it != it.m_end; it++)
        {
            cout << *it << " ";
        }
        cout << endl;
    }
    // { // НЕ РАБОТАЕТ !!!
    //     auto it = SpecialIterator::sorter<SpecialIterator::etDESCENDING>(input, input + size);
    //     cout << "Test array [size=" << size << "]" << endl;
    //     for (; it != it.m_end; it++)
    //     {
    //         cout << *it << " ";
    //     }
    //     cout << endl;
    // }

    //5
    /*
     * Положим, что у нас есть структура, работников предприятия.
     */
    struct Emploee {
        int      number;
        string   lastName;
        string   firstName;
        long     salary;
    };

    ptrdiff_t ss;
    Emploee AcmeContractors[] =
    {
        {25433, "Bunny", "Bugs" , 2000},
        {31200, "Duck",  "Duffy", 2500},
        {62310, "Pig" ,  "Porky", 1900}
    };
    size_t acmeSize = sizeof AcmeContractors / sizeof *AcmeContractors;
    {   // Выводим всех сотрудников по фамилиям
        auto it = SpecialIterator::field_walker(AcmeContractors, fieldof(Emploee, lastName));
        for (; it != SpecialIterator::field_walker(AcmeContractors + acmeSize, fieldof(Emploee, lastName)); ++it)
        {
            cout << *it << endl;
        }
    }
    {   // Выводим всех сотрудников по именам
        auto it = SpecialIterator::field_walker(AcmeContractors, fieldof(Emploee, firstName));
        for (; it != SpecialIterator::field_walker(AcmeContractors + acmeSize, fieldof(Emploee, firstName)); ++it)
        {
            cout << *it << endl;
        }
    }
    {   // Выводим всех сотрудников по зарплатам
        auto it = SpecialIterator::field_walker(AcmeContractors, fieldof(Emploee, salary));
        for (; it != SpecialIterator::field_walker(AcmeContractors + acmeSize, fieldof(Emploee, salary)); ++it)
        {
            cout << *it << endl;
        }
    }

    //6
    // TODO: пофиксить ошибки в алгоритме
    /* Допустим дана такая четырехбайтовая последовательность
     */
    char inputBytes[] = "\x0A\x0B\x0A\x0B";
    // Выведем ее по битам
    // 00001010 00001011 00001010 00001011
    auto it = SpecialIterator::bit_walker(inputBytes);
    short delimCounter = 0;
    for (; it != SpecialIterator::bit_walker(inputBytes + 1); ++it)
    {
        cout << *it;
        delimCounter += 1;
        if (delimCounter % 8 == 0)
            cout << " ";
    }
    cout << endl;

    return 0;
}