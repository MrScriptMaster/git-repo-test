#include <utility>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>

class CAudiotunePackage
{
public:
    CAudiotunePackage(const std::string& sPath)
        : m_packageSize(1499)
        ,m_File(sPath)
    {}
    CAudiotunePackage(std::size_t packageSize, const std::string& sPath)
        : m_packageSize(packageSize)
        ,m_File(sPath)
    {}
    virtual ~CAudiotunePackage()
    {
        if (m_File.is_open())
        {
            m_File.close();
        }
    }
    /**
     * @brief Get the next package
     * 
     * @return Get package or NULL
     */
    const char* getPackage()
    {
        if (!m_File.is_open())
        {
            return NULL;
        }
        m_sBuffer = m_sResidue;
        m_sResidue.clear();
        char delimeter = '\n';
        char maskingCharacter = ',';

        while (std::getline(m_File, m_sResidue, delimeter))
        {
            // truncate very long strings
            if (m_sResidue.size() > m_packageSize)
                m_sResidue.resize(m_packageSize);
            
            // Triming
            [](std::string& s)
            {
                const char whitespace[] {" \t\n"};
                const std::size_t first(s.find_first_not_of(whitespace));
                if (std::string::npos == first) { return; }
                const std::size_t last(s.find_last_not_of(whitespace));
                s = std::move(s.substr(first, (last - first + 1)));
            }(m_sResidue);
            
            // Remove extra spaces
            [](std::string& s)
            {
                std::string sTmp;
                std::unique_copy (s.begin(), s.end(),
                                    std::back_insert_iterator<std::string>(sTmp),
                                    [](char a,char b){ return isspace(a) && isspace(b);});
                s = std::move(sTmp);
            }(m_sResidue);

            // Replace spaces to masking character 
            // (note: the client truncates the message by the first space,
            //  so we mask them like that)
            [&maskingCharacter](std::string& s)
            {
                std::replace_if(s.begin(), s.end(),
                                    [](char c)->bool { return (isspace(c)); }, maskingCharacter);
            }(m_sResidue);

            // Append delimeter back
            m_sResidue += delimeter;
            if ((m_sBuffer.size() + m_sResidue.size()) < m_packageSize)
            {
                m_sBuffer.append(m_sResidue);
                m_sResidue.clear();
            }
            else
            {
                break;
            }
        }

        if (m_sBuffer.empty())
        {
            return NULL;
        }
        return m_sBuffer.c_str();
    }
    void Reset(const std::string& sPath)
    {
        if (m_File.is_open())
        {
            m_File.close();
            m_File.open(sPath);
        }
        m_sBuffer.clear();
        m_sResidue.clear();
    }
private:
    std::size_t m_packageSize;
    std::string m_sBuffer;
    std::string m_sResidue;
    std::ifstream m_File;
};

int main()
{
    const char* pack;
    CAudiotunePackage f(73, "test.txt");
    while ((pack = f.getPackage()) != NULL)
    {
        std::cout << "------- package --------\n" 
                  << pack << std::endl;
    }
    return 0;
}
