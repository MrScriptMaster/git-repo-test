/**
 * @file debug_api.c
 * @brief API для логгирования.
 * @version 0.1
 * @date 2020-03-11
 * 
 * Программный интерфейс для системы логгирования для проекта C/C++.
 * 
 */

#ifndef _DEBUG_API_H
#define _DEBUG_API_H

#if defined(__cplusplus)
extern "C"
{
#endif

/**
 * @brief Выводит сообщение в стандартный поток ошибок.
 * 
 * @param log_level Уровень логгирования.
 * @param file Имя файла.
 * @param line_number Номер строки в файле.
 * @param msg Форматная строка.
 * @param ... Переменные в соответствии с форматной строкой.
 */
extern void __debug_print(int log_level, char* file, int line_number, char* msg, ...);
extern void add_debug_level(int level);
extern void remove_debug_level(int level);
extern void set_need_source(int);
extern int set_debug_level(int level);
extern int check_debug_level(int level);
extern void allocate_mutex();


/*
 * Для многопоточных программ реализуйте интерфейсы для мьютексов
 */

typedef void*                       OS_SPECIFIC_MUTEX;

#define NO_IMPLEMENTED

#ifdef NO_IMPLEMENTED
    void __no_implementation(OS_SPECIFIC_MUTEX, const char* action_desc);
    #undef DUMMY
    #define DUMMY(x,y) __no_implementation(x,y)
    #define OS_SPECIFIC_MUTEX_CREATE(x)  DUMMY(x,"Create mutex")
    #define OS_SPECIFIC_MUTEX_WAIT(x)    DUMMY(x,"Lock mutex")
    #define OS_SPECIFIC_MUTEX_RELEASE(x) DUMMY(x,"Unlock mutex")
#else
    #ifdef WIN32
        #define OS_SPECIFIC_MUTEX_CREATE            
        #define OS_SPECIFIC_MUTEX_WAIT              
        #define OS_SPECIFIC_MUTEX_RELEASE           
    #elif defined(__unix__)
        #define OS_SPECIFIC_MUTEX_CREATE            
        #define OS_SPECIFIC_MUTEX_WAIT              
        #define OS_SPECIFIC_MUTEX_RELEASE           
    #else
        #error "API is not supported on this platfotm"
    #endif
#endif

#if defined(__cplusplus)
}
#endif

#endif