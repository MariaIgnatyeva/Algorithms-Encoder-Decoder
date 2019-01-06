#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <utility>
#include <algorithm>
#include <string>
#include <stdlib.h>
#include "Algorithm.h"
#include "Decoding.h"
#include "Encoding.h"

#ifndef KDZ_HUFFMAN_H
#define KDZ_HUFFMAN_H

// Вершина дерева
class Node
{
public:
    // Количество появления символа в алфавите
    int quantity;

    // Номер алфавитного символа
    char ch;

    // Левый ребенок
    Node* leftChild;

    // Правый ребенок
    Node* rightChild;

    // Для создания вершин - символов алфавита
    Node(char c, int n) : quantity(n),
                          ch(c),
                          leftChild(nullptr),
                          rightChild(nullptr) {}

    // Для создания вспомогательных вершин
    Node(Node* l, Node* r) : quantity(l->quantity + r->quantity),
                             leftChild(l),
                             rightChild(r) {}

    /**
     * Освобождает память из очереди с приоритетами
     */
    void deleteNodes()
    {
        if (!rightChild && !leftChild)
        {
            delete this;
            return;
        }

        if (leftChild)
            leftChild->deleteNodes();

        if (rightChild)
            rightChild->deleteNodes();
    }
};

// Comparator of queue with priority
struct Comparator
{
    bool operator()(Node* left, Node* right)
    {
        return left->quantity > right->quantity;
    }
};

class Huffman : public IAlgorithm
{
private:
    // Коды Хаффмана для символов алфавита
    std::map<char, std::string> codes;

    // Вершины дерева для алгоритма Хаффмана
    std::priority_queue<Node*, std::vector<Node*>, Comparator> codeTree;

    std::map<char, unsigned int> quantities;

    void emptyFields()
    {
        codes = {};
        codeTree = {};
        quantities = {};
    }

    void countQuantities(const std::vector<char>& data)
    {
        quantities = {};

        for (const auto& ch : data)
            quantities[ch]++;
    }

    /**
     * Добавляет символ алфавита в изначальное дерево.
     * @param chance сколько раз символ встречается в сообщении
     */
    void addSymbols(const std::map<char, unsigned int>& quantities)
    {
        codeTree = {};

        for (const auto& item : quantities)
            codeTree.push(new Node(item.first, item.second));
    }

    Node* buildTree(const std::map<char, unsigned int>& quantities)
    {
        addSymbols(quantities);

        // Строим дерево
        while (codeTree.size() > 1)
        {
            Node* left = codeTree.top();
            codeTree.pop();

            Node* right = codeTree.top();
            codeTree.pop();

            Node* newNode = new Node(left, right);

            codeTree.push(newNode);
        }

        return codeTree.top();
    }

    /**
     * Строит оптимальный код для символов
     * @param nd вершина, с которой обходим дерево
     * @param code код, который уже имеет вершина nd
     */
    void createCodes(const Node* nd, std::string prefix)
    {
        // Дошли до листа дерева (искомых символов алфавита)
        if (!nd->rightChild && !nd->leftChild)
        {
            if (prefix == "")
                prefix = "1";

            codes[nd->ch] = prefix;
            return;
        }

        // Уход влево дает 1 к коду
        if (nd->leftChild)
            createCodes(nd->leftChild, prefix + "0");

        // Уход вправо дает 0 к коду
        if (nd->rightChild)
            createCodes(nd->rightChild, prefix + "1");
    }

public:
    ~Huffman()
    {
        if (codeTree.size() > 0)
            codeTree.top()->deleteNodes();
    }

    void encode(const std::string& inputPath, const std::string& outputPath) override
    {
        emptyFields();

        std::vector<char> bytes = FileReaderWriter::getBytesFromFile(inputPath);

        countQuantities(bytes);

        codes = {};
        createCodes(buildTree(quantities), "");

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

            createCodes(buildTree(quantities), "");

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

#endif //KDZ_HUFFMAN_H
