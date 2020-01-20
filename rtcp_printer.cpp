#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <iostream>
#include <iomanip>

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
        size_t nBytes = resize(nBits); // m_nSize updated
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

typedef unsigned long int u_int32;
struct RTCPReceiverBlock
{
	u_int32 ssrc;
	u_int32 fractionLost	: 8;
	u_int32 cummulativeLost	: 24;
	u_int32 highestSeq;
	u_int32 jitter;
	u_int32 lastTimeStamp;
	u_int32 delay;
};

std::string print(const RTCPReceiverBlock& block, bool title = false) {
    size_t offset = 0;
    std::stringstream ss;
    size_t line = 0;
    if (title)
        ss << "(RTCP) Receiver block ---------------------------" << std::endl;
    do {
        ss << std::setw(8) << std::setfill('0') << offset << std::setw(0)  << ": ";
        if (offset == 32) {
            u_int32 merged_value = (((block.fractionLost & 0x000000FF) << 24) | (block.cummulativeLost & 0x00FFFFFF)) ;
            ss << Cbitset(8, block.fractionLost).to_string() << Cbitset(24, block.cummulativeLost).to_string() 
               << "(0x" << std::setw(8) << std::setfill('0') << std::hex << merged_value << ")" << std::dec;
        } else {
            switch (line)
            {
                case 0: ss << Cbitset(sizeof(u_int32)*8, block.ssrc).to_string() 
                           << "(0x" << std::setw(8) << std::setfill('0') << std::hex << block.ssrc << std::setw(0) << ")" << std::dec; break;
                case 2: ss << Cbitset(sizeof(u_int32)*8, block.highestSeq).to_string() 
                           << "(0x" << std::setw(8) << std::setfill('0') << std::hex << block.highestSeq << std::setw(0) << ")" << std::dec; break;
                case 3: ss << Cbitset(sizeof(u_int32)*8, block.jitter).to_string() 
                           << "(0x" << std::setw(8) << std::setfill('0') << std::hex << block.jitter << std::setw(0) << ")" << std::dec; break;
                case 4: ss << Cbitset(sizeof(u_int32)*8, block.lastTimeStamp).to_string() 
                           << "(0x" << std::setw(8) << std::setfill('0') << std::hex << block.lastTimeStamp << std::setw(0) << ")" << std::dec; break;
                case 5: ss << Cbitset(sizeof(u_int32)*8, block.delay).to_string()
                           << "(0x" << std::setw(8) << std::setfill('0') <<  std::hex << block.delay << std::setw(0) << ")" << std::dec; break;
            }
        }
        ss << std::endl;
        offset += sizeof(uint32_t) * 8;
    } while ( ++line < sizeof(RTCPReceiverBlock) / sizeof(uint32_t) );
    return ss.str();
}

using namespace std;

int main(int argc, char** argv) {

    //cout << Cbitset(sizeof(long) * 8, 256).to_string(8) << endl;

    RTCPReceiverBlock test { 1,2,3,4,5,6,7 };

    cout << print(test, true);

    return 0;
}
