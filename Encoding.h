#include <iostream>
#include <map>
#include <vector>
#include "FileReaderWriter.h"

#ifndef KDZ_ENCODING_H
#define KDZ_ENCODING_H

class Encoding
{
public:
    static const size_t CHAR_SIZE = FileReaderWriter::CHAR_SIZE;

    static std::string encodeQuantityTable(const std::map<char, unsigned int>& quantities)
    {
        std::string res = "";

        res += (quantities.size() - 1);

        for (const auto& q : quantities)
        {
            res += q.first;

            char one = static_cast<int8_t>(q.second / (CHAR_SIZE * CHAR_SIZE * CHAR_SIZE));
            char two = static_cast<int8_t>((q.second - one * (CHAR_SIZE * CHAR_SIZE * CHAR_SIZE)) / (CHAR_SIZE * CHAR_SIZE));
            char three = static_cast<int8_t>((q.second - one * (CHAR_SIZE * CHAR_SIZE * CHAR_SIZE) - two * (CHAR_SIZE * CHAR_SIZE)) / CHAR_SIZE);
            char four = static_cast<int8_t>(q.second - one * CHAR_SIZE * CHAR_SIZE * CHAR_SIZE - two * CHAR_SIZE * CHAR_SIZE - three * CHAR_SIZE);

            res += one;
            res += two;
            res += three;
            res += four;
        }

        return res;
    }

    static std::tuple<char, int8_t> parseBinaryStringToChar(std::string& str)
    {
        int8_t n = 0;
        int8_t zerosAtEnd = 0;

        if (str.size() >= 8)
            for (int j = 0; j < 8; ++j)
            {
                n = n | ((str[0] - '0') << (7 - j));
                str.erase(0, 1);
            }

        else if (str.size() > 0)
            {
                int size = str.size();

                for (int i = 0; i < 8; ++i)
                {
                    if (i < size)
                    {
                        n = n | ((str[0] - '0') << (7 - i));
                        str.erase(0, 1);
                    }
                    else
                    {
                        n = n | (false << (7 - i));
                        zerosAtEnd++;
                    }
                }
            }

        return std::make_tuple(char(n), zerosAtEnd);
    }

    static void encodeFile(const std::vector<char>& bytes, const std::map<char, unsigned int> quantities,
                                  const std::map<char, std::string>& codes, const std::string outputPath)
    {
        std::string quantityTable = encodeQuantityTable(quantities);

        std::string encodedText;

        int8_t zerosAtEnd = 0;

        std::string code;

        for (int i = 0; i < bytes.size(); ++i)
        {
            code += codes.at(bytes[i]);

            while (code.size() >= 8)
                encodedText += std::get<0>(parseBinaryStringToChar(code));
        }

        if (code.size() > 0)
        {
            char encodedSymbol;
            std::tie(encodedSymbol, zerosAtEnd) = parseBinaryStringToChar(code);
            encodedText += encodedSymbol;
        }

        FileReaderWriter::writeElementsToFile(quantityTable, zerosAtEnd, encodedText, outputPath);
    }
};

#endif //KDZ_ENCODING_H
