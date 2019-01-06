#include <iostream>
#include <vector>
#include "Algorithm.h"
#include "FileReaderWriter.h"

#ifndef KDZ_LZ77_H
#define KDZ_LZ77_H

class LZ77 : public IAlgorithm
{
private:
    static const size_t K_BYTE = 1024;

    size_t histBufMaxSize = 0;

    size_t prevBufMaxSize = 0;

    struct Node
    {
        int offset;
        size_t length;
        char next;

        Node(size_t o, size_t l, char c)
        {
            offset = o;
            length = l;
            next = c;
        }
    };

    Node createNode(const std::vector<char>& s, size_t histBufInd, size_t prevBufInd, size_t right)
    {
        size_t maxLength = 0;
        size_t maxInd = prevBufInd;

        for (; histBufInd < prevBufInd; ++histBufInd)
        {
            size_t length = 0;
            size_t ind = histBufInd;

            while (s[ind] == s[prevBufInd + length] && prevBufInd + length < right && ind < right)
            {
                length++;
                ind++;
            }

            if (length >= maxLength && length > 0)
            {
                maxLength = length;
                maxInd = histBufInd;
            }
        }

        return {prevBufInd - maxInd, maxLength, s[prevBufInd + maxLength]};
    }

public:
    LZ77(size_t hist, size_t prev)
    {
        histBufMaxSize = hist * K_BYTE;
        prevBufMaxSize = prev * K_BYTE;
    }

    void encode(const std::string& inputPath, const std::string& outputPath) override
    {
        std::vector<char> text = FileReaderWriter::getBytesFromFile(inputPath);

        size_t left = 0;
        size_t right = prevBufMaxSize > text.size() ? text.size() : prevBufMaxSize;

        size_t prevBufInd = 0;

        std::ofstream fout(outputPath, std::ios::binary);

        while (prevBufInd < text.size())
        {
            Node nd = createNode(text, left, prevBufInd, right);

            prevBufInd += nd.length + 1;

            if (prevBufInd >= text.size() && nd.next != text[text.size() - 1])
                nd.offset *= -1;

            std::string encodedText;

            encodedText += nd.offset / CHAR_SIZE;
            encodedText += nd.offset - nd.offset / CHAR_SIZE * CHAR_SIZE;

            encodedText += nd.length / CHAR_SIZE;
            encodedText += nd.length - nd.length / CHAR_SIZE * CHAR_SIZE;

            encodedText += nd.next;

            fout.write(encodedText.c_str(), encodedText.length());

            // Recalculating both window borders
            if (prevBufInd - histBufMaxSize > 0)
                left = prevBufInd - histBufMaxSize;
            else
                left = 0;

            if (prevBufInd + prevBufMaxSize <= text.size())
                right = prevBufInd + prevBufMaxSize;
            else
                right = text.size();
        }

        fout.close();
    }

    void decode(const std::string& inputPath, const std::string& outputPath) override
    {
        std::vector<char> bytes = FileReaderWriter::getBytesFromFile(inputPath);

        std::ofstream fout(outputPath, std::ios::binary);

        std::string decodedText;

        for (int i = 0; i < bytes.size(); i += 5)
        {
            int offset = bytes[i] * CHAR_SIZE + static_cast<uint8_t>(bytes[i + 1]);
            size_t length = bytes[i + 2] * CHAR_SIZE + static_cast<uint8_t>(bytes[i + 3]);
            char ch = bytes[i + 4];

            bool isLast = false;
            if (offset < 0)
            {
                offset *= -1;
                isLast = true;
            }

            if (length > 0)
            {
                int start = decodedText.length() - offset;

                for (int j = 0; j < length; ++j)
                    decodedText += decodedText[start + j];
            }

            if (!isLast)
                decodedText += ch;
        }

        fout.write(decodedText.c_str(), decodedText.length());

        fout.close();
    }
};

#endif //KDZ_LZ77_H
