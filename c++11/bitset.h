#ifndef CBITSET_H
#define CBITSET_H

#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <sstream>

namespace Toolkit {

/**
 * @brief Dynamic bitset. Allows you to represent an integer in binary form.
 * 
 */
class Cbitset {
protected:
    size_t m_nBits;
    std::vector<uint8_t> m_Data;
public:
    virtual ~Cbitset() = default;
    Cbitset(Cbitset &&) = default;
    Cbitset(const Cbitset &) = default;
    
    /**
     * @brief Construct a new Cbitset object
     * 
     * @param nBits Number of bits for the value
     * @param value 
     */
    explicit Cbitset(size_t nBits, uint64_t value)
    : m_nBits(0)
    {
        size_t nBytes = resize(nBits); // m_nBits updated
        std::fill_n(m_Data.begin(), m_Data.size(), 0);
        if (m_nBits < sizeof(uint64_t)) {
            assert(nBytes < 3);
            uint64_t maskedValue = value & ((1 << (m_nBits + 1)) - 1);
            for (size_t i = 0; i < m_Data.size(); i++) {
                m_Data[i] = (maskedValue >> (i * 8)) & 0xff;
            }
        } else {
            for (size_t i = 0; i < sizeof(uint64_t); i++) {
                m_Data[i] = (value >> (i * 8)) & 0xff;
            }
        }
    }

    size_t         bits_num() const { return m_nBits; }
    size_t         size()           { return m_Data.size(); }
    const uint8_t* data()     const { return m_Data.data(); }
    uint8_t*       data()           { return m_Data.data(); }

    /**
     * @brief Reallocates the container size for the number.
     * 
     * @param nBits Desired number of bits of the stored number.
     * @return Size of the current container in bytes
     */
    size_t resize(size_t nBits) {
        m_nBits = nBits;
        size_t nBytes;
        if (m_nBits < 8) {
            nBytes = 1;
        } else {
            nBytes = 1 + (m_nBits - 1) / 8;
        }
        m_Data.resize(nBytes);
        return nBytes;
    }

    /**
     * @brief Returns true if a bit on specified position is set.
     * 
     * @param pos Bit position starting from left
     * @return true if bit on the pos is set
     * @return false if bit on the pos is unset
     */
    bool operator[](size_t pos) const {
        size_t byte = pos / 8, offset = pos % 8;
        return (m_Data[byte] >> offset) & 0x1;
    }

    /**
     * @brief Returns a binary number as a string.
     * 
     * @param blk Sets the size of the block of bits between which a space
     * will be affixed for better visual perception.
     * @return std::string 
     */
    std::string to_string(unsigned int blk = 4) const {
        std::stringstream ss;
        bool formatting = (blk > 0 && blk < m_nBits) ? true : false;
        for (size_t i = 0; i < m_nBits; i++) {
            ss << ((*this)[m_nBits - i - 1] ? "1" : "0");
            if (formatting && !((i+1) % blk)) {
                ss << " ";
            }
        }
        return ss.str();
    }

    unsigned long long to_udec() const {
        unsigned long long mult = 1;
        unsigned long long result = 0;
        for (size_t i = 0; i < m_nBits - 2; i++) {
            if ((*this)[i]) {
                result += mult;
            }
            mult *= 2;
        }
        return result;
    }

    unsigned long long to_dec() const {
        unsigned long long mult = 1;
        unsigned long long result = 0;
        for (size_t i = 0; i < m_nBits - 1; i++) {
            if ((*this)[i]) {
                result += mult;
            }
            mult *= 2;
        }
        return result;
    }

}; // class Cbitset

}

#endif
