//#pragma once

#include <string>
#include <cstring>
#include <stack>
#include <fstream>

namespace Nfstream {
    
    class CIf;
    class CElse;
    class CEndIf;
    class Indented;
    class CEndl;
    class Fill;
    class AFile;

    typedef AFile  _fstream_type;

    _fstream_type& operator<< (_fstream_type&, const CIf&);
    _fstream_type& operator<< (_fstream_type&, const CElse&);
    _fstream_type& operator<< (_fstream_type&, const CEndIf&);
    _fstream_type& operator<< (_fstream_type&, Indented&&);
    _fstream_type& operator<< (_fstream_type&, const CEndl&);
    _fstream_type& operator<< (_fstream_type&, const Fill&);

    class Indented
    {
        std::string m_string;
        char m_delimeter;
    public:
        Indented(const std::string& string, const char delim = '\n')
            : m_string(string)
            ,m_delimeter(delim)
        {}
        friend _fstream_type& operator<< (_fstream_type&, Indented&&);
    };

    class AFile
    {
    public:       
        AFile();
        virtual ~AFile() = default;

        virtual _fstream_type&
        operator<< (char) =0;

        virtual _fstream_type&
        operator<< (const char*) =0;

        virtual _fstream_type&
        operator<< (const std::string&) =0;

        virtual _fstream_type&
        operator<< (int) =0;

    protected:
        std::stack<bool> m_allowed;
        uint m_currentColumn;
        bool is_write_suppressed() const;
        bool invariant() const;

        friend _fstream_type& operator<< (_fstream_type&, const CIf&);
        friend _fstream_type& operator<< (_fstream_type&, const CElse&);
        friend _fstream_type& operator<< (_fstream_type&, const CEndIf&);
        friend _fstream_type& operator<< (_fstream_type&, Indented&&);
        friend _fstream_type& operator<< (_fstream_type&, const CEndl&);
        friend _fstream_type& operator<< (_fstream_type&, const Fill&);
    };

    class File : public AFile
    {
        std::ofstream m_ostream;

    public:
        explicit
        File(const std::string& path);
        ~File();

        _fstream_type&
        operator<< (char) override;

        _fstream_type&
        operator<< (const char*) override;

        _fstream_type&
        operator<< (const std::string&) override;

        _fstream_type&
        operator<< (int) override;
    };

    class CIf
    {
    public:
        bool m_condition;
        CIf (bool condition) : m_condition(condition) {}
    };

    class CElse {};
    extern CElse Else;
    class CEndIf {};
    extern CEndIf EndIf;
    class CEndl {};
    extern CEndl Endl;
    class Fill
    {
    public:
        size_t m_fillSize;
        char m_filler;
        Fill(size_t size, char filler = ' ')
            : m_filler(filler)
            ,m_fillSize(size)
        {}
    };

    const CIf If(bool);
    Indented Indent(const std::string&, const char delim = '\n');
    Fill Filler(size_t, char filler = ' ');
}

// --- cut off ---------------------------------------------------- cut off ---

#include <cstring>
#include <vector>
#include <sstream>

using namespace std;

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

namespace Nfstream
{

CElse Else;
CEndIf EndIf;
CEndl Endl;

AFile::AFile()
    : m_currentColumn(0)
{
    m_allowed.push(true);
}

bool AFile::is_write_suppressed() const 
{ 
    return !m_allowed.top(); 
}

bool AFile::invariant() const 
{ 
    return !m_allowed.empty(); 
}

File::File(const std::string& path)
    : AFile()
{
    m_ostream.open(path, ios::out | ios::trunc);
}

File::~File()
{
    if (m_ostream.is_open()) m_ostream.close();
}

_fstream_type& File::operator<< (char c)
{
    if (is_write_suppressed()) return static_cast<_fstream_type&>(*this);
    m_ostream << c;
    m_currentColumn++;
    if ('\n' == c)  m_currentColumn = 0;
    return static_cast<_fstream_type&>(*this);
}

_fstream_type& File::operator<< (const char* mass)
{
    if (is_write_suppressed()) return static_cast<_fstream_type&>(*this);
    for (size_t i = 0; mass[i] != '\0'; i++) {
        *this << mass[i];
    }
    return static_cast<_fstream_type&>(*this);
}

_fstream_type& File::operator<< (const std::string& str)
{
    if (is_write_suppressed()) return static_cast<_fstream_type&>(*this);
    m_ostream << str;
    for (std::string::const_reverse_iterator it = str.rbegin(); 
        it != str.rend(); it++)
    {
        if (*it == '\n') {
            m_currentColumn = 0;
            break;
        }
        m_currentColumn++;
    }
    return static_cast<_fstream_type&>(*this);
}

_fstream_type& File::operator<< (int value)
{
    if (is_write_suppressed()) return static_cast<_fstream_type&>(*this);
    char buffer[256];
    math::itoa(value, buffer);
    *this << string(buffer);
    return static_cast<_fstream_type&>(*this);
}

const CIf If (bool cond)
{
    return CIf(cond);
}

Indented Indent(const std::string& input, const char delim)
{
    return Indented(input, delim);
}

Fill Filler(size_t size, char filler)
{
    return Fill(size, filler);
}

_fstream_type& operator<< (_fstream_type& stream, const CIf& _if)
{
    stream.m_allowed.push(_if.m_condition && stream.m_allowed.top());
    return stream;
}

_fstream_type& operator<< (_fstream_type& stream, const CElse& _else)
{
    bool lastAllowed = stream.m_allowed.top();
    stream.m_allowed.pop();
    stream.m_allowed.push(!lastAllowed && stream.m_allowed.top());
    return stream;
}

_fstream_type& operator<< (_fstream_type& stream, const CEndIf& _endif)
{
    stream.m_allowed.pop();
    return stream;
}

_fstream_type& operator<< (_fstream_type& stream, Indented&& _ind)
{
    [] (string& input, size_t indSize, char delim = '\n') {
        stringstream ss(input);
        string tstr, space;
        vector<string> elems;
        while (getline(ss, tstr, delim)) {
            elems.push_back(tstr);
        }
        tstr.clear();
        space.resize(indSize, ' ');
        size_t elemSize = elems.size();
        if (elemSize > 1)
        {
            size_t i = 0;
            do {
                tstr += elems[i] + delim + space;
            } while(++i != elemSize - 1);
            tstr += elems[elemSize - 1];
        }
        else if (elemSize > 0)
            tstr += elems[elemSize - 1];
        input = tstr;
    } (_ind.m_string, stream.m_currentColumn, _ind.m_delimeter);
    stream << _ind.m_string;
    return stream;
}

_fstream_type& operator<< (_fstream_type& stream, const CEndl& _endl)
{
    stream << '\n';
    return stream;
}

_fstream_type& operator<< (_fstream_type& stream, const Fill& _filler)
{
    string tstr;
    tstr.resize(_filler.m_fillSize, _filler.m_filler);
    stream << tstr;
    return stream;
}

} // Nfstream

// --- cut off ---------------------------------------------------- cut off ---

#include <iostream>

using namespace Nfstream;

int main(int argc, char* argv[])
{
    File file("test.txt");
    file 
        << 'c'
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
        << string("Hello again\n   ") 
        << "   " << Indent("Alpha\nBeta\nGamma") << Endl
        << Filler(3,'-') << "Number = " << 15 << Endl;
    return 0;
}