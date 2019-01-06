#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <map>
#include "Algorithm.h"
#include "Decoding.h"
#include "Encoding.h"

#ifndef KDZ_SHANNONFANO_H
#define KDZ_SHANNONFANO_H

class ShannonFano : public IAlgorithm
{
public:
    struct Node
    {
        char ch;
        unsigned int quantity = 0;

        Node (char c, unsigned int q)
        {
            ch = c;
            quantity = q;
        }
    };

    std::vector<Node> nodes;

    unsigned int totalAmount = 0;

    std::map<char, unsigned int> quantities;

    std::map<char, std::string> codes;

    void emptyFields()
    {
        nodes = {};
        totalAmount = 0;
        quantities = {};
        codes = {};
    }

    void countQuantities(const std::vector<char>& data)
    {
        totalAmount = data.size();

        for (const auto& ch : data)
            quantities[ch]++;
    }

    void createNodes()
    {
        for (const auto& q : quantities)
            nodes.push_back({q.first, q.second});

        std::sort(begin(nodes), end(nodes), [](Node lhs, Node rhs)
        {
            return lhs.quantity > rhs.quantity;
        });
    }

    /**
     * Алгоритм кодирования Шеннона-Фано
     * @param left индекс начала рассмотрения массива (включительно)
     * @param right индекс конца рассмотрения массива (не включительно)
     * @param sum сумма на рассматриваемом участке массива
     */
    void createCodes(int left, int right, int sum)
    {
        // Если только один символ
        if (left + 1 == right)
            return;

        // Если два символа
        if (left + 2 == right)
        {
            codes[nodes[left].ch] += "0";
            codes[nodes[right - 1].ch] += "1";

            return;
        }

        // Индекс, на котором массив разделится на 2 части
        int index = left;

        // Суммы частей массива
        int leftSum = 0;
        int rightSum = sum;

        // Разница между суммами
        int difference;

        // Поиск разделяющего элемента
        do
        {
            leftSum += nodes[index].quantity;
            rightSum = sum - leftSum;

            difference = abs(rightSum - leftSum);

            // Генерация кода для элементов первой части
            codes[nodes[index++].ch] += "0";

        } while (abs(rightSum - 2 * static_cast<int>(nodes[index].quantity) - leftSum) < difference);

        // Генерация кода для элементов второй части
        for (int i = index; i < right; ++i)
            codes[nodes[i].ch] += '1';

        // Проходим по первой половине
        createCodes(left, index, leftSum);

        // Проходим по второй половине
        createCodes(index, right, rightSum);
    }

public:
    void encode(const std::string& inputPath, const std::string& outputPath) override
    {
        emptyFields();

        std::vector<char> bytes = FileReaderWriter::getBytesFromFile(inputPath);

        countQuantities(bytes);
        
        createNodes();

        createCodes(0, nodes.size(), totalAmount);

        Encoding::encodeFile(bytes, quantities, codes, outputPath);
    }

    void decode(const std::string& inputPath, const std::string& outputPath) override
    {
        emptyFields();

        std::ifstream fin(inputPath, std::ios::ate | std::ios::binary);

        if (fin)
        {
            const int64_t length = fin.tellg();

            fin.seekg(0, std::ios::beg);

            int64_t readBytes = 0;

            quantities = Decoding::decodeQuantityTable(fin, readBytes);

            for (const auto& q : quantities)
                totalAmount += q.second;

            createNodes();

            createCodes(0, nodes.size(), totalAmount);

            std::map<std::string, char> decodes = Decoding::createDecodes(codes);

            uint8_t zerosAtEnd;
            fin.read((char*)(&zerosAtEnd), sizeof(char));
            readBytes++;

            std::string decodedText = Decoding::decodeRemaining(fin, length, readBytes, zerosAtEnd, decodes);

            FileReaderWriter::writeStringToFile(decodedText, outputPath);

            fin.close();
        }
    }
};

#endif //KDZ_SHANNONFANO_H
