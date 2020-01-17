
#include <ctime>
#include <iostream>
#include <ostream>
#include <bitset>

/*
 * Author:  David Robert Nadeau
 * Site:    http://NadeauSoftware.com/
 * License: Creative Commons Attribution 3.0 Unported License
 *          http://creativecommons.org/licenses/by/3.0/deed.en_US
 */

// #if defined(_WIN32)
// #include <Windows.h>

// #elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
// #include <unistd.h>
// #include <sys/resource.h>
// #include <sys/times.h>
// #include <time.h>

// #else
// #error "Unable to define getCPUTime( ) for an unknown OS."
// #endif

// /**
//  * Returns the amount of CPU time used by the current process,
//  * in seconds, or -1.0 if an error occurred.
//  */
// double getCPUTime( )
// {
// #if defined(_WIN32)
//     /* Windows -------------------------------------------------- */
//     FILETIME createTime;
//     FILETIME exitTime;
//     FILETIME kernelTime;
//     FILETIME userTime;
//     if ( GetProcessTimes( GetCurrentProcess( ),
//         &createTime, &exitTime, &kernelTime, &userTime ) != -1 )
//     {
//         SYSTEMTIME userSystemTime;
//         if ( FileTimeToSystemTime( &userTime, &userSystemTime ) != -1 )
//             return (double)userSystemTime.wHour * 3600.0 +
//                 (double)userSystemTime.wMinute * 60.0 +
//                 (double)userSystemTime.wSecond +
//                 (double)userSystemTime.wMilliseconds / 1000.0;
//     }

// #elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
//     /* AIX, BSD, Cygwin, HP-UX, Linux, OSX, and Solaris --------- */

// #if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
//     /* Prefer high-res POSIX timers, when available. */
//     {
//         clockid_t id;
//         struct timespec ts;
// #if _POSIX_CPUTIME > 0
//         /* Clock ids vary by OS.  Query the id, if possible. */
//         if ( clock_getcpuclockid( 0, &id ) == -1 )
// #endif
// #if defined(CLOCK_PROCESS_CPUTIME_ID)
//             /* Use known clock id for AIX, Linux, or Solaris. */
//             id = CLOCK_PROCESS_CPUTIME_ID;
// #elif defined(CLOCK_VIRTUAL)
//             /* Use known clock id for BSD or HP-UX. */
//             id = CLOCK_VIRTUAL;
// #else
//             id = (clockid_t)-1;
// #endif
//         if ( id != (clockid_t)-1 && clock_gettime( id, &ts ) != -1 )
//             return (double)ts.tv_sec +
//                 (double)ts.tv_nsec / 1000000000.0;
//     }
// #endif

// #if defined(RUSAGE_SELF)
//     {
//         struct rusage rusage;
//         if ( getrusage( RUSAGE_SELF, &rusage ) != -1 )
//             return (double)rusage.ru_utime.tv_sec +
//                 (double)rusage.ru_utime.tv_usec / 1000000.0;
//     }
// #endif

// #if defined(_SC_CLK_TCK)
//     {
//         const double ticks = (double)sysconf( _SC_CLK_TCK );
//         struct tms tms;
//         if ( times( &tms ) != (clock_t)-1 )
//             return (double)tms.tms_utime / ticks;
//     }
// #endif

// #if defined(CLOCKS_PER_SEC)
//     {
//         clock_t cl = clock( );
//         if ( cl != (clock_t)-1 )
//             return (double)cl / (double)CLOCKS_PER_SEC;
//     }
// #endif

// #endif

//     return -1;      /* Failed. */
// }

class NTPTimeStamp {
public:
    unsigned long secs;
    unsigned long frac; // micro secs

    NTPTimeStamp() : secs(0), frac(0) {}

    NTPTimeStamp operator- (const NTPTimeStamp &rhs) const {
        NTPTimeStamp res;
        unsigned long frac_1 = frac >> 12,
                      frac_2 = rhs.frac >> 12;
        res.secs = secs - rhs.secs;
        if (frac_1 < frac_2)
        {
            frac_1 += 1000000;
            res.secs -= 1;
        }
        res.frac = frac_1 - frac_2;
        res.frac <<= 12;
        return res;
    }

    bool operator> (const NTPTimeStamp &rhs) const {
        bool rc = false;
        unsigned long frac_1 = frac >> 12,
                      frac_2 = rhs.frac >> 12;
        if (secs > rhs.secs) {
            rc = true;
        } else if (secs < rhs.secs) {
            rc = false;
        } else if (frac_1 > frac_2) {
            rc = true;
        } else {
            rc = false;
        }
        return rc;
    }
};

using namespace std;

ostream& operator<< (ostream& o, const NTPTimeStamp& ts) {
    return o << "Timestamp: " << endl
      << "  secs = " << ts.secs << " (" << bitset<sizeof(ts.secs) * 8>(ts.secs) << ")" << endl
      << "  frac = " << ts.frac << " (" << bitset<sizeof(ts.frac) * 8>(ts.frac) << ")" << endl;
}

double conv(const NTPTimeStamp& ts) {
    return (double)(1000000ULL * ts.secs + (ts.frac >> 12)) / 1000000;
}
double conv1(const NTPTimeStamp& ts) {
    return (double)(1000000LL * ts.secs + (ts.frac >> 12)) / 1000000;
}
unsigned long conv2(const NTPTimeStamp& ts) {
    return ((1000000ULL * ts.secs + (ts.frac >> 12))) / 1000000;
}
unsigned long conv3(const NTPTimeStamp& ts) {
    return ((1000000LL * ts.secs + (ts.frac >> 12))) / 1000000;
}

#define COEF 12

int main(int argc, char* argv[]) {

    NTPTimeStamp t1, t2, t3, t4;
    //t1.secs = 0xb710;
    t1.secs = 0xafc8;
    t2.frac = 0x8000;
    
    t2.secs = 0x0005;
    t2.frac = 0x4000;

    //t3.secs = 0xb705;
    t3.secs = 0x2706;
    //t3.secs = 0xafc8;
    t3.frac = 0x2000;

    cout << "BEFORE:\n" << t1 << t2 << t3;
    
    t4 = t1 - t2 - t3;

    cout << "AFTER:\n" << t4
         << "double: " << conv(t4) * 1000 << endl
         << "double1: " << conv1(t4) * 1000 << endl
         << "u_long: " << conv2(t4) * 1000 << endl
         << "u_long1: " << conv3(t4) * 1000 << endl
         << "test: " << (0UL - 1) << endl
        ;

    return 0;
}

/*

ts32.secs = ts.secs & 0x0000FFFF;
ts32.frac = ts.frac & 0xFFFF0000;
lsr32.secs = (rb.lastTimeStamp & 0xFFFF0000) >> 16;
lsr32.frac = (rb.lastTimeStamp & 0x0000FFFF) << 16;

*/
