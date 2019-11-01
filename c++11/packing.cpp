#include <iostream>
#include <utility>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <vector>


#define MASKED_SPACE ','

class CPacker
{
public:
    using string_processing = std::function<void(std::string&)>;
    using iterator = std::vector<string_processing>::iterator;

    //Helpers
    static string_processing trimmer()
    {
        return [](std::string& s)
            {
                const char whitespace[] {" \t\n"};
                const std::size_t first(s.find_first_not_of(whitespace));
                if (std::string::npos == first) { return; }
                const std::size_t last(s.find_last_not_of(whitespace));
                s = std::move(s.substr(first, (last - first + 1)));
            };
    }
    static string_processing extra_space_remover()
    {
        return [](std::string& s)
            {
                std::string sTmp;
                std::unique_copy (s.begin(), s.end(),
                                    std::back_insert_iterator<std::string>(sTmp),
                                    [](char a,char b){ return isspace(a) && isspace(b);});
                s = std::move(sTmp);
            };
    }
    static string_processing append_new_line()
    {
        return [](std::string& s)
            {
                s += "\n";
            };
    }
    static string_processing space_masking()
    {
        return [](std::string& s)
        {
            std::replace_if(s.begin(), s.end(),
                [](char c)->bool { return (isspace(c)); }, MASKED_SPACE);
        };
    }
    
    CPacker(const std::string& sTarget, bool bIsFilePath = true)
        : m_packageSize(1499)
        ,m_bFileStream(bIsFilePath)
    {
        if (isFileStream())
        {
            m_File.open(sTarget);
        }
        else
        {
            m_sStream = std::istringstream(sTarget);
        }
    }
    CPacker(std::size_t packageSize, const std::string& sTarget, bool bIsFilePath = true)
        : m_packageSize(packageSize)
        ,m_bFileStream(bIsFilePath)
    {
        m_packageSize = std::max((std::size_t)1, m_packageSize);
        if (isFileStream())
        {
            m_File.open(sTarget);
        }
        else
        {
            m_sStream = std::move(std::istringstream(sTarget));
        }
    }
    virtual ~CPacker()
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
        std::istream* pS = NULL;
        if (isFileStream())
        {
            if (!m_File.is_open())
            {
                return NULL;
            }
            pS = static_cast<std::istream*>(&m_File);
        }
        else
        {
            pS = static_cast<std::istream*>(&m_sStream);
        }

        m_sBuffer = m_sResidue;
        m_sResidue.clear();

        while (std::getline(*pS, m_sResidue))
        {           
            for (iterator it = m_vProcess.begin(); it != m_vProcess.end(); ++it)
            {
                (*it)(m_sResidue);
            }

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
    void Reset(const std::string& sTarget, bool bIsFilePath = true)
    {
        m_bFileStream = bIsFilePath;
        if (isFileStream())
        {
            if (m_File.is_open())
            {
                m_File.close();
                m_File.open(sTarget);
            }
        }
        else
        {
            m_sStream.clear();
            m_sStream = std::move(std::istringstream(sTarget));
        }
        m_sBuffer.clear();
        m_sResidue.clear();
    }
    bool isFileStream() const { return (m_bFileStream != false); }
    CPacker& addStringProcessing(string_processing&& p)
    {
        m_vProcess.push_back(std::move(p));
        return *this;
    }

private:
    std::size_t m_packageSize;
    std::string m_sBuffer;
    std::string m_sResidue;
    std::ifstream m_File;
    std::istringstream m_sStream;
    bool m_bFileStream;
    std::vector<string_processing> m_vProcess;
};

int main()
{
    const char* pack;
    CPacker f("test.txt",true);
    
    f.addStringProcessing(CPacker::trimmer())
     .addStringProcessing(CPacker::extra_space_remover())
     .addStringProcessing(CPacker::space_masking())
     .addStringProcessing(CPacker::append_new_line());
    
    while ((pack = f.getPackage()) != NULL)
    {
        std::cout << "------- package --------\n" 
                  << pack << std::endl;
    }
    return 0;
}
