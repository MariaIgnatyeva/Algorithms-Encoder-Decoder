#include <iostream>
#include <map>
#include "FileReaderWriter.h"

#ifndef KDZ_DECODING_H
#define KDZ_DECODING_H

class Decoding
{
public:
    static const size_t CHAR_SIZE = FileReaderWriter::CHAR_SIZE;

    static std::map<char, unsigned int> decodeQuantityTable(std::ifstream& fin, int64_t& readBytes)
    {
        std::map<char, unsigned int> quantities;

        uint8_t size;
        fin.read((char*)(&size), sizeof(char));
        readBytes++;

        for (int i = 0; i <= size; ++i)
        {
            char ch;
            readBytes++;
            fin.read(&ch, sizeof(char));

            uint8_t* q = new uint8_t[4];
            readBytes += 4;
            fin.read((char*) q, sizeof(char) * 4);

            quantities[ch] = q[0] * 256u * 256u * 256u + q[1] * 256u * 256u + q[2] * 256u + q[3];

            delete[] q;
        }

        return quantities;
    }

    static std::map<std::string, char> createDecodes(const std::map<char, std::string>& codes)
    {
        std::map<std::string, char> decodes;

        for (const auto& item : codes)
            decodes[item.second] = item.first;

        return decodes;
    }

    static std::string decodeRemaining(std::ifstream& fin, int64_t length, int64_t readBytes, uint8_t zerosAtEnd,
                                       const std::map<std::string, char>& decodes)
    {
        std::string decodedText;

        std::string str;
        char ch;

        while (readBytes < length)
        {
            fin.read(&ch, sizeof(char));
            readBytes++;

            int n = 8;

            if (readBytes == length)
                n = n - zerosAtEnd;

            for (int i = 0; i < n; i++)
                str += std::to_string((ch & (1 << (7 - i))) > 0);

            for (size_t j = 0; j < str.size(); ++j)
            {
                std::string code = str.substr(0, j + 1);

                if (decodes.count(code) == 1)
                {
                    decodedText += decodes.at(code);
                    str.erase(0, j + 1);
                    j = SIZE_MAX;
                }
            }
        }

        return decodedText;
    }
};

#endif //KDZ_DECODING_H
