#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>


using namespace std;

void readFile1(const char *fileName)
{
    std::ifstream ifs(fileName, std::ios::binary);
    std::string s;
    copy(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>(), back_inserter(s));

    std::cout << " " << s.length() << " ";
}

void readFile2(const char *fileName)
{
    std::ifstream file(fileName, std::ios::binary);
    std::string str;

    std::copy(
        istream_iterator<char>(file), 
        istream_iterator<char>(), 
        insert_iterator<string>(str,str.begin()));

    std::cout << " " << str.length() << " ";
}

void readFile3(const char *fileName)
{
    std::ifstream file(fileName, std::ios::binary);
    string str;

    file.seekg( 0, ios_base::end);
    std::ifstream::pos_type len = file.tellg();
    file.seekg( 0);
    str.resize( len);
    file.read( (char*)str.data(), len); 

    std::cout << " " << str.length() << " ";
}

void readFile4(const char *fileName)
{
    string s;
    ifstream inp(fileName, std::ios::binary);
    getline(inp, s, '\0');

    std::cout << " " << s.length() << " ";
}

void readFile5(const char *fileName)
{
    string s;
    ifstream is(fileName, std::ios::binary);

    //
    // attempt to grow string buffer to match file size,
    // this doesn't always work...
    s.reserve(is.rdbuf()->in_avail());
    char c;
    while(is.get(c))
    {
        // use logarithmic growth stategy, in case
        // in_avail (above) returned zero:
        if(s.capacity() == s.size())
            s.reserve(s.capacity() * 3);
        s.append(1, c);
    }

    std::cout << " " << s.length() << " ";
}

void readFile6(const char *fileName)
{
    typedef std::istream_iterator<char> IIC;

    std::ifstream file(fileName, std::ios::binary);    

    file.seekg( 0, std::ios_base::end);
    std::fstream::pos_type size = file.tellg();
    file.seekg( 0);
    
    std::string str_file;
    str_file.reserve(size);
    std::copy(IIC(file), IIC(), std::back_inserter(str_file));

    std::cout << " " << str_file.length() << " ";

}

void readFile7(const char *fileName)
{
    std::ifstream in(fileName, std::ios::binary);
    ostringstream out;           
    out << in.rdbuf(); 

    string s;
    out.str().swap(s); 

    std::cout << " " << s.length() << " ";
}

typedef void (*PF)(const char*);

class FReadFile
{
public:
    FReadFile(const char *fileName)
        :    fileName(fileName)
    {
    }

    void operator()(PF pf) const
    {
        pf(fileName);
    }

private:
    const char *fileName;
};

int main() 
{ 
    

    const size_t readersCount = 7;
    PF readers[readersCount] = {
        readFile1, readFile2, readFile3, 
        readFile4, readFile5, readFile6, 
        readFile7
    };
    
    FReadFile func("c:\\temp\\prof\\test.txt");

    for(int i = 0; i < 10; ++i)
    {
        std::for_each(readers, readers + readersCount, func);
        std::cout << i << "\n";
    }    

}
