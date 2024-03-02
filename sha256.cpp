#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <bitset>
#include <fstream>

using namespace std;

uint32_t rotateRight(uint32_t value, uint32_t shift)
{
    return (value >> shift) | (value << (32 - shift));
}

uint32_t choice(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (~x & z);
}

uint32_t majority(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

uint32_t upperSigma0(uint32_t x) { return rotateRight(x, 2) ^ rotateRight(x, 13) ^ rotateRight(x, 22); }
uint32_t upperSigma1(uint32_t x) { return rotateRight(x, 6) ^ rotateRight(x, 11) ^ rotateRight(x, 25); }
uint32_t lowerSigma0(uint32_t x) { return rotateRight(x, 7) ^ rotateRight(x, 18) ^ (x >> 3); }
uint32_t lowerSigma1(uint32_t x) { return rotateRight(x, 17) ^ rotateRight(x, 19) ^ (x >> 10); }

const uint32_t constants[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

void padMessage(string &input, vector<uint8_t> &output)
{
    uint64_t bitLength = input.size() * 8;

    for (int i = 0; i < input.size(); i++)
    {
        char c = input[i];
        output.push_back(c);
    }

    output.push_back(0x80);

    int paddingZeros = 448 - (bitLength + 1) % 512;
    paddingZeros = (paddingZeros < 0) ? paddingZeros + 512 : paddingZeros;
    for (int i = 0; i < paddingZeros / 8; i++)
    {
        output.push_back(0x00);
    }

    for (int i = 0; i < 8; ++i)
    {
        output.push_back((bitLength >> (56 - i * 8)) & 0xFF);
    }
}

int main()
{
    std::ifstream inputFile("text");
    if (!inputFile)
    {
        std::cerr << "File not exists";
        return 1;
    }

    std::string inputText;
    std::string line;
    while (std::getline(inputFile, line))
    {
        inputText += line + "\n";
    }
    inputFile.close();

    vector<uint8_t> paddedInput;
    padMessage(inputText, paddedInput);

    uint32_t hash[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

    for (int i = 0; i * 64 < paddedInput.size(); i++)
    {
        uint32_t words[64];
        for (int t = 0; t < 16; t++)
        {
            words[t] = (paddedInput[i * 64 + t * 4] << 24) |
                       (paddedInput[i * 64 + t * 4 + 1] << 16) |
                       (paddedInput[i * 64 + t * 4 + 2] << 8) |
                       (paddedInput[i * 64 + t * 4 + 3]);
        }
        for (int t = 16; t < 64; t++)
        {
            words[t] = lowerSigma1(words[t - 2]) + words[t - 7] + lowerSigma0(words[t - 15]) + words[t - 16];
        }

        uint32_t a = hash[0], b = hash[1], c = hash[2], d = hash[3];
        uint32_t e = hash[4], f = hash[5], g = hash[6], h = hash[7];

        for (int t = 0; t < 64; t++)
        {
            uint32_t T1 = h + upperSigma1(e) + choice(e, f, g) + constants[t] + words[t];
            uint32_t T2 = upperSigma0(a) + majority(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;
        }

        hash[0] += a;
        hash[1] += b;
        hash[2] += c;
        hash[3] += d;
        hash[4] += e;
        hash[5] += f;
        hash[6] += g;
        hash[7] += h;
    }

    for (int i = 0; i < 8; i++)
    {
        cout << hex << setfill('0') << setw(8) << hash[i];
    }
    cout << endl;

    return 0;
}