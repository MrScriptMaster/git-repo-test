#include <cassert>
#include <fstream>
#include <list>
#include <stack>
#include <string>

/*
 * ATTENTION: Not debugged
 */

class BaseStream;

namespace Manips {
    /*
     * If
     */
    class CIf {
    public:
        bool m_condition;
        CIf(bool condition) : m_condition(condition) {}
    }; // If
    const CIf If(bool codition);
    BaseStream& operator << (BaseStream&, const CIf&);
    /*
     * Else
     */
    class  CElse {};
    extern CElse Else;
    BaseStream& operator << (BaseStream&, const CElse&);
    /*
     * EndIf
     */
    class CEndIf {};
    extern CEndIf EndIf;
    BaseStream& operator << (BaseStream&, const CEndIf&);
    /*
     * CIntend
     */
    class CIndented;
    BaseStream& operator << (BaseStream&, CIndented&&);
    class CIndented {
        std::string m_string;
    public:
        CIndented(const std::string& string)
            : m_string(string)
        {}
        friend BaseStream& operator << (BaseStream&, CIndented&&);
    };
    CIndented Indent(const std::string&);
    /*
     * CEndl
     */
    class CEndl {};
    extern CEndl Endl;
    BaseStream& operator << (BaseStream&, const CEndl&);
} // namespace Manips

class BaseStream {
public:
    BaseStream();
    virtual ~BaseStream() = default;
           BaseStream& operator << (char);
           BaseStream& operator << (const char*);
    inline BaseStream& operator << (const std::string&);
           BaseStream& operator << (int);

protected:
    virtual void write(char) =0;
    uint m_currentColumn;
    inline bool is_write_suppressed() const
    {
        return !m_allowed.top();
    }
    inline bool invariant() const
    {
        return !m_allowed.empty();
    }

    std::stack<bool> m_allowed;
    friend BaseStream& Manips::operator<< (BaseStream&, const Manips::CIf&);
    friend BaseStream& Manips::operator<< (BaseStream&, const Manips::CElse&);
    friend BaseStream& Manips::operator<< (BaseStream&, const Manips::CEndIf&);
    friend BaseStream& Manips::operator<< (BaseStream&, Manips::CIndented&&);
    friend BaseStream& Manips::operator<< (BaseStream&, const Manips::CEndl&);
};

class CustomStream : public BaseStream {
    std::string   m_streamName;
    std::ofstream m_ostr;

public:
    CustomStream(const char* streamName);
    ~CustomStream();

protected:
    virtual void write(char) override;
}; // CustomStream

// --- cut off ---------------------------------------------------- cut off ---

#include <cstring>

using namespace std;

//
// BaseStream
//

namespace math {
    void reverse(char s[])
    {
        int i, j;
        char c;
    
        for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
            c = s[i];
            s[i] = s[j];
            s[j] = c;
        }
    }
    void itoa(int n, char s[])
    {
        int i, sign;
    
        if ((sign = n) < 0)
            n = -n;
        i = 0;
        do {
            s[i++] = n % 10 + '0';
        } while ((n /= 10) > 0);
        if (sign < 0)
            s[i++] = '-';
        s[i] = '\0';
        reverse(s);
    }
}

BaseStream::BaseStream()
    : m_currentColumn(0)
{
    m_allowed.push(true);
    assert(invariant());
}

BaseStream& BaseStream::operator<< (char c)
{
    assert(invariant());
    if (is_write_suppressed()) return *this;
    write(c);
    m_currentColumn++;
    if ('\n' == c)  m_currentColumn = 0;
    assert(invariant());
    return *this;
}

BaseStream& BaseStream::operator<< (const char* cMass)
{
    if (is_write_suppressed()) return *this;
    for (size_t i = 0; cMass[i] != '\0'; i++) {
        *this << cMass[i];
    }
    return *this;
}

BaseStream& BaseStream::operator<< (const string& input)
{
    return ((*this) << input.c_str());
}

BaseStream& BaseStream::operator<< (int value)
{
    char buffer[256];
    math::itoa(value, buffer);
    return ((*this) << string(buffer) );
}

//
// CustomStream
//

CustomStream::CustomStream(const char* streamName)
    : BaseStream()
    ,m_streamName(streamName)
{
    m_ostr.open(m_streamName, std::ios::out | std::ios::trunc);
}

CustomStream::~CustomStream()
{
    if (m_ostr.is_open())
        m_ostr.close();
}

void CustomStream::write(char c)
{
    m_ostr << c;
}

//
// If
//

const Manips::CIf Manips::If(bool condition)
{
    return Manips::CIf(condition);
}

BaseStream& Manips::operator<< (BaseStream& stream, const Manips::CIf& If)
{
    assert(stream.invariant());
    stream.m_allowed.push(If.m_condition && stream.m_allowed.top());
    assert(stream.invariant());
    return stream;
}

//
// Else
//

Manips::CElse Manips::Else;
BaseStream& Manips::operator<< (BaseStream& stream, const Manips::CElse& Else)
{
    assert(stream.invariant());
    bool lastAllowed = stream.m_allowed.top();
    stream.m_allowed.pop();
    stream.m_allowed.push(!lastAllowed && stream.m_allowed.top());
    assert(stream.invariant());
    return stream;
}

//
// EndIf
//

Manips::CEndIf Manips::EndIf;

BaseStream& Manips::operator << (BaseStream& stream, const Manips::CEndIf& EndIf)
{
    assert(stream.invariant());
    stream.m_allowed.pop();
    assert(stream.invariant());
    return stream;
}

//
// CIndented
//

BaseStream& Manips::operator<< (BaseStream& stream, Manips::CIndented&& input)
{
    assert(stream.invariant());
    uint curColumn = stream.m_currentColumn;
    for (size_t i = 0; i < input.m_string.size(); i++)
    {
        char c = input.m_string[i];
        stream << c;
        if (c == '\n') {
            for (size_t j = 0; j < curColumn; j++) {
                stream << ' ';
            }
        }
    }
    assert(stream.invariant());
    return stream;
}

Manips::CIndented Manips::Indent(const std::string& inputString)
{
    return CIndented(inputString);
}

//
// CEndl
//

Manips::CEndl Manips::Endl;
BaseStream& Manips::operator<< (BaseStream& stream, const Manips::CEndl& endl)
{
    stream << '\n';
    return stream;
}

// --- cut off ---------------------------------------------------- cut off ---

#include <iostream>

using namespace Manips;

int main(int argc, char* argv[])
{
    cout << "--- Start write to file ---" << endl;
    CustomStream TestFile("test.txt");
    TestFile
        << "Hello, World!" << Endl
        << If (true)
        <<   "Print this 1" << Endl
        << EndIf
        << If (false)
        <<   "Do not print this" << Endl
        << EndIf
        << If (false)
        <<    "Do not print this" << Endl
        << Else
        <<    "Print this 2" << Endl
        << EndIf
        << "   " << Indent("Alpha\nBeta\nGamma") << Endl
        << "Number = " << 15 << Endl;

    cout << "--- End write to file ---" << endl;
    return 0;
}