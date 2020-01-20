
#include <ctime>
#include <iostream>
#include <ostream>
#include <bitset>
#include <cstdio>

#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <sstream>

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

void test_1() {
    NTPTimeStamp t1, t2, t3, t4;
    // 46864.500 (s)
    t1.secs = 0xb710;
    t1.frac = 0x8000;
    // 5.250 (s)
    t2.secs = 0x0005;
    t2.frac = 0x4000;
    // 46853.125 (s)
    t3.secs = 0xb705;
    t3.frac = 0x2000;

    cout << "TEST_1: Expected value: 6.125 (s) ~ 6 (s)" << endl;
    cout << "BEFORE:\n" << t1 << t2 << t3;
    
    t4 = t1 - t2 - t3;

    cout << "AFTER:\n" << t4
         << "double: " << conv(t4) * 1000 << " (ms) " << conv(t4) << " (s)" << endl
         << "double1: " << conv1(t4) * 1000 << " (ms) " << conv1(t4) << " (s)" << endl
         << "u_long: " << conv2(t4) * 1000 << " (ms) " << conv2(t4) << " (s)" << endl
         << "u_long1: " << conv3(t4) * 1000 << " (ms) " << conv3(t4) << " (s)" << endl
         //<< "test: " << (0UL - 1) << endl
        ;
}

void test_2() {
    unsigned long long timestamp = 0xb7108000;
    timestamp <<= 16;
    cout << "TEST 2" << endl;
    cout << "(0000000000000000101101110001000010000000000000000000000000000000) : " << bitset<sizeof timestamp * 8>(timestamp) << endl;
    cout << "SHIFTING" << endl;
    unsigned long time = (timestamp >> 16);
    cout << "(10110111000100001000000000000000): " << bitset<sizeof time * 8>(time) << endl;
    NTPTimeStamp ts1, ts2, ts3, ts4;
    ts1.secs = (time & 0xFFFF0000) >> 16;
    ts1.frac = (time & 0x0000FFFF) << 16;
    
    ts3 = ts4 = ts1;
    
    ts2.secs = time & 0xFFFF0000;
    ts2.frac = time & 0x0000FFFF;

    ts3.secs &= 0x0000FFFF;
    ts3.frac &= 0xFFFF0000;

    cout << ts1 
//       << ts2
         << ts3
         << "Test 1: " << bitset<sizeof (unsigned long) * 8>( ((ts4.secs & 0x0000FFFF) << 16) | ((ts4.frac & 0xFFFF0000) >> 16) )
    ;
}

class Cbitset {
protected:
    size_t m_nSize;
    std::vector<uint8_t> m_Data;
public:
    virtual ~Cbitset() = default;
    Cbitset(Cbitset &&) = default;
    Cbitset(const Cbitset &) = default;
    explicit Cbitset(size_t nBits, uint64_t value)
    : m_nSize(0)
    {
        size_t nBytes = resize(nBits);;
        std::fill_n(m_Data.begin(), m_Data.size(), 0);
        if (m_nSize < sizeof(uint64_t)) {
            assert(nBytes < 3);
            uint64_t maskedValue = value & ((1 << (m_nSize + 1)) - 1);
            for (size_t i = 0; i < m_Data.size(); i++) {
                m_Data[i] = (maskedValue >> (i * 8)) & 0xff;
            }
        } else {
            for (size_t i = 0; i < sizeof(uint64_t); i++) {
                m_Data[i] = (value >> (i * 8)) & 0xff;
            }
        }
    }

    size_t         bits_num() const { return m_nSize; }
    size_t         size()           { return m_Data.size(); }
    const uint8_t* data()     const { return m_Data.data(); }
    uint8_t*       data()           { return m_Data.data(); }

    size_t resize(size_t nBits) {
        m_nSize = nBits;
        size_t nBytes;
        if (m_nSize < 8) {
            nBytes = 1;
        } else {
            nBytes = 1 + (m_nSize - 1) / 8;
        }
        m_Data.resize(nBytes);
        return nBytes;
    }

    bool operator[](size_t pos) const {
        size_t byte = pos / 8, offset = pos % 8;
        return (m_Data[byte] >> offset) & 0x1;
    }

    std::string to_string(unsigned int blk = 4) const {
        std::stringstream ss;
        bool formatting = (blk > 0 && blk < m_nSize) ? true : false;
        for (size_t i = 0; i < m_nSize; i++) {
            ss << ((*this)[m_nSize - i - 1] ? "1" : "0");
            if (formatting && !((i+1) % blk)) {
                ss << " ";
            }
        }
        return ss.str();
    }
};

int main(int argc, char* argv[]) {

    cout << Cbitset(sizeof(int) * 8, 0xb710).to_string() << endl;
    //test_1();
    //test_2();
    //bitset<sizeof(int) * 8> tt(5);
    //printf("Test: %s\n", tt.to_string().c_str());

    return 0;
}

/*

ts32.secs = ts.secs & 0x0000FFFF;
ts32.frac = ts.frac & 0xFFFF0000;
lsr32.secs = (rb.lastTimeStamp & 0xFFFF0000) >> 16;
lsr32.frac = (rb.lastTimeStamp & 0x0000FFFF) << 16;

*/
