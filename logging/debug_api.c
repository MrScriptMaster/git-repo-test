
#include <stdio.h>
#include <stdarg.h>

#include "debug_api.h"

#ifdef NO_IMPLEMENTED
void __no_implementation(OS_SPECIFIC_MUTEX mutex, const char* action_desc)
{
    if (action_desc != NULL)
    {
        printf("%s: %s\n", __FUNCTION__, action_desc);
    }
}
#endif

#define MAX_LEVEL  20
typedef int bool;

static OS_SPECIFIC_MUTEX __mutex = NULL;

static bool g_need_source_info = 0;
static bool g_need_lock        = 0;
static int g_debug_level[MAX_LEVEL] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static int g_debug_level_count = 0;

static void __api_lock_mutex(void) { OS_SPECIFIC_MUTEX_WAIT(__mutex);  }
static void __api_unlock_mutex(void) { OS_SPECIFIC_MUTEX_RELEASE(__mutex); }


static int get_debug_level() {
    if (g_debug_level_count == 0) {
        printf("logger: no debug level is set\n");
    } else {
        for (int i = 0; i < g_debug_level_count; i++) {
            printf("logger: debug_level[%d] = %d\n", i, g_debug_level[i]);
        }
    }
}

int set_debug_level(int level)
{
    for (int i = 0; i < MAX_LEVEL; i++)
    {
        g_debug_level[i] = 0;
    }
    if (level == 0)
    {
        g_debug_level_count = 0;
    }
    else
    {
        g_debug_level_count = 1;
        g_debug_level[0] = level;
    }
    get_debug_level();
}

void add_debug_level(int level)
{
    int i = 0;
    for (i = 0; i < g_debug_level_count; i++) {
        if (level > g_debug_level[i])  continue;
        if (level == g_debug_level[i]) return;
        break;
    }
    if (i == g_debug_level_count)
    {
        g_debug_level[i] = level;
    }
    else
    {
        for (int j = g_debug_level_count; j > i; j--)
        {
            g_debug_level[j] = g_debug_level[j-1];
        }
        g_debug_level[i] = level;
    }
    g_debug_level_count++;
    get_debug_level();
}

int check_debug_level(int level)
{
    if (g_debug_level_count == 0) return -1;
    for (int i = 0; i < g_debug_level_count; i++) {
        if (level > g_debug_level[i])  continue;
        if (level == g_debug_level[i]) return i;
        return -1;
    }
    return -1;
}

void remove_debug_level(int level)
{
    int i = check_debug_level(level);
    if (i < 0) return;
    else {
        for (int j = i; i < g_debug_level_count-1; j++)
        {
            g_debug_level[j] = g_debug_level[j+1];
        }
        g_debug_level[g_debug_level_count-1] = 0;
        g_debug_level_count--;
    }
    get_debug_level();
}

void set_need_source(int value)
{
    g_need_source_info = value;
}

void allocate_mutex()
{
    g_need_lock = 1;
}

void __debug_print(int log_level, char* file, int line_number, char* msg, ...)
{
    va_list(p);
    bool lv = 0;

    if (check_debug_level(log_level) >= 0)
    {
        if (g_need_lock)
        {
            if (!__mutex) OS_SPECIFIC_MUTEX_CREATE(&__mutex);
            __api_lock_mutex();
            lv = 1;
        }
        va_start(p, msg);
        vfprintf(stderr, msg, p);
        va_end(p);
        if (g_need_source_info && file)
        {
            fprintf(stderr, "\t\t\t%s (%d)\r\n", file , line_number);
        }

        if (lv) __api_unlock_mutex();
    }
}
