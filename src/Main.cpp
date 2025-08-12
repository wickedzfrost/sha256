#include <array>
#include <bit>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

using Binary = std::vector<std::uint8_t>;

std::vector<std::uint8_t> toBinary(const std::string& message)
{
    return std::vector<std::uint8_t>{message.begin(), message.end()};
}

template<typename T>
int getMessageBitSize(const T& message)
{
    return static_cast<int>(message.size()) * 8;
}

void appendBit(Binary& binary, const std::uint8_t& bitsToAppend)
{
    binary.push_back(bitsToAppend);
}

Binary preprocess(Binary& binary, const std::uint64_t& originalMessageBitSize)
{
    // Append 1 to the end
    constexpr std::uint8_t oneBit{ 0x80 };
    appendBit(binary, oneBit);

    int currentBitSize{ getMessageBitSize(binary) };

    // Ensure the data is a multiple of 512
    while (currentBitSize % 512 != 0)
    {
        binary.push_back(std::uint8_t{ 0x0 });
        currentBitSize += 8;
    }

    // Append 64 bit big-endian integer representing length of message in binary
    auto iterator{ binary.rbegin() };
    for (int i{ 0 }; i < 8; ++i)
    {
        *iterator = uint8_t{ originalMessageBitSize >> (i * 8) & 0xFF };
        ++iterator;
    }

    return binary;
}

std::uint32_t sigmaZero(const std::uint32_t& word)
{
    if (word == 0)
        return word;

    return std::rotr(word, 7) ^ std::rotr(word, 18) ^ (word >> 3);
}

std::uint32_t sigmaOne(const std::uint32_t& word)
{
    if (word == 0)
        return word;

    return std::rotr(word, 17) ^ std::rotr(word, 19) ^ (word >> 10);
}

// binary is multiple of 512
std::array<std::uint32_t, 64> createMessageSchedule(const Binary& binary)
{
    std::array<std::uint32_t, 64> messageBlock{};

    {
        std::uint32_t word{};
        std::size_t counter{ 0 };
        for (int i{ 0 }; i < 64; ++i)
        {
            // Whatever the fuck this is?????
            word |= binary[i] << (4 - ((i + 1) % 4) % 4) * 8;
    
            if ((i + 1) % 4 == 0)
            {
                messageBlock[counter] = word;
                word = uint32_t{};
                ++counter;
            }
        }
    }

    for (int i{ 16 }; i < 64; ++i)
    {
        messageBlock[i] = sigmaOne(messageBlock[i - 2]) + messageBlock[i - 7] + sigmaZero(messageBlock[i - 15]) + messageBlock[i - 16];
    }

    for (std::uint32_t word : messageBlock)
        std::cout << "Word: " << std::bitset<32>(word) << '\n';

    return messageBlock;
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

    Binary binary{ toBinary(message) };
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