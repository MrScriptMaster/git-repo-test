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
 * @date 2019-02-05
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
 *  - Изменяемые итераторы - это сочетание итератора вывода с итератором из любой вышеперечисленной категории
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
    CIterator() : m_ptr(nullptr) {}
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
        ItType m_begin;         /// итератор на последний элемент
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
            // Когда еще не было ни одного прохода, ищем следующий элемент сначала
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
            // Когда еще не было ни одного прохода, ищем следующий элемент сначала
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

    return 0;
}