#include <array>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

using Bytes = std::vector<std::uint8_t>;
using Binary = std::vector<std::bitset<8>>;

/*
    Converts a string into binary
*/
std::vector<std::uint8_t> toBinary2(const std::string& message)
{
    return std::vector<std::uint8_t>{message.begin(), message.end()};
}

Bytes toBytes(const std::string& message)
{
    return Bytes{ message.begin(), message.end() };
}

Binary toBinary(const Bytes& bytes)
{
    Binary binaryVector{};

    for (std::uint8_t byte : bytes)
    {
        std::bitset<8> bits{ byte };
        binaryVector.push_back(bits);
    }

    return binaryVector;
}

template<typename T>
int getMessageBitSize(const T message)
{
    return static_cast<int>(message.size()) * 8;
}

// Might remove later
// 
//void printReverse(const Binary& binary)
//{
//    std::cout << "Reverse: \n";
//
//    int counter{ 0 };
//    for (auto it{ binary.rbegin() }; it != binary.rend(); ++it)
//    {
//        std::cout << *it << ' ';
//        ++counter;
//
//        if (counter % 8 == 0)
//            std::cout << '\n';
//    }
//
//    std::cout << "\n\n";
//}

template<int N>
void appendBit(Binary& binary, const std::bitset<N> bitsToAppend)
{
    binary.push_back(bitsToAppend);
}

Binary preprocess(Binary& binary, const std::uint64_t originalMessageBitSize)
{
    // Append 1 to the end
    constexpr std::bitset<8> oneBit{ 0x80 };
    appendBit(binary, oneBit);

    int currentBitSize{ getMessageBitSize(binary) };

    // Ensure the data is a multiple of 512
    while (currentBitSize % 512 != 0)
    {
        binary.push_back(std::bitset<8>{ 0x0 });
        currentBitSize += 8;
    }

    // Append 64 bit big-endian integer representing length of message in binary
    auto iter{ binary.rbegin() };
    for (int i{ 0 }; i < 8; ++i)
    {
        const std::bitset<8> byte{ uint8_t{ originalMessageBitSize >> (i * 8) & 0xFF } };
        *iter = byte;
        ++iter;
    }

    return binary;
}

std::vector<std::bitset<32>> createMessageSchedule(const Binary& binary)
{
    std::vector<std::bitset<32>> messageSchedule{};
    
    std::bitset<32> word{};
    for (int i{ 0 }; i < 64; ++i)
    {
        // Whatever the fuck this is?????
        uint32_t data = binary[i].to_ulong() << (4 - ((i + 1) % 4) % 4) * 8;
        word |= std::bitset<32>{ data };
    
        if ((i + 1) % 4 == 0)
        {
            messageSchedule.push_back(word);
            word.reset();
        }
    }

    for(std::bitset<32> byte : messageSchedule)
        std::cout << "Word: " << byte << '\n';

    return messageSchedule;
}

int main(int argc, [[maybe_unused]] char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: ./sha256 [Message]\n";
        
        return 1;
    }

    std::cout << "Enter some text: ";
    std::string message{};
    std::getline(std::cin >> std::ws, message);
    
    const int messageBitSize{ getMessageBitSize(message) };

    Binary binary{ toBinary(toBytes(message)) };
    binary = preprocess(binary, messageBitSize);
    
    int counter{ 0 };
    for (std::bitset<8> byte : binary)
    {
        std::cout << byte << ' ';
        ++counter;

        if (counter % 4 == 0)
            std::cout << '\n';
    }

    std::cout << '\n';

    std::cout << "Number of bits: " << getMessageBitSize(binary) << '\n';

    createMessageSchedule(binary);

    /*
        Steps to produce some hash!

        1. Convert message to binary!

        2. Preprocessing
        2.1 Padding the message

        3. Parse the message

        4. 
    */

    constexpr std::array<std::uint32_t, 8> sqrtHash
    {
        0x6a09e667,
        0xbb67ae85,
        0x3c6ef372,
        0xa54ff53a,
        0x510e527f,
        0x9b05688c,
        0x1f83d9ab,
        0x5be0cd19,
    };

    constexpr std::array<std::uint32_t, 64> cbrtHash
    {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
    };

    return 0;
}