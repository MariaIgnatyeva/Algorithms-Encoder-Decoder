/*
 * КДЗ по дисциплине Алгоритмы и структуры данных 2017-2018 уч.год
 * Игнатьева Мария, группа БПИ162, 25.03.2018
 * Среда разработки: CLion 2017 1.3
 * Состав проекта: main.cpp
 *                 Huffman.h, ShannonFano.h, LZ77.h,
 *                 Encoding.h, Decoding.h, FileReaderWriter.h
 * Сделано: алгоритм Хаффмана - кодирование и декодирование
 *          алгоритм Щеннона-Фано - кодирование и декодирование
 *          алгоритм LZ77 - кодирование и декодирование
 *          эксперимент на 36 файлах с записью результатов
 *          отчет прилагается
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <bitset>
#include <map>
#include <cmath>
#include <chrono>
#include "Huffman.h"
#include "ShannonFano.h"
#include "LZ77.h"

using gettime = std::chrono::high_resolution_clock;
using ns = std::chrono::nanoseconds;

double countEntropyAndQuantities(const std::string& path, std::map<char, double>& quantities)
{
    std::vector<char> bytes = FileReaderWriter::getBytesFromFile(path);
    
    for (const auto& ch : bytes)
        quantities[ch] += 1 / (double)(bytes.size());

    double entropy = 0;

    for (const auto& q : quantities)
        entropy -= (q.second * (log(q.second) / log(bytes.size())));

    return entropy;
}

int main()
{
    Huffman h;
    ShannonFano sh;
    LZ77 lz1(4, 1), lz2(8, 2), lz3(16, 4);

    std::vector<IAlgorithm*> algos = {&h, &sh, &lz1, &lz2, &lz3};

    std::string encodedPaths[] = {".haff", ".shan", ".lz775", ".lz7710", ".lz7720"};
    std::string decodedPaths[] = {".unhaff", ".unshan", ".unlz775", ".unlz7710", ".unlz7720"};

    const int ITER_NUM = 1;

    std::ofstream foutRes("results.csv");
    foutRes << ";;Huffman;;;Shannon-Fano;;;LZ77_5;;;LZ77_10;;;LZ77_20;;\n";

    std::string fields = ";K;tp;tu";
    foutRes << "File;H" << fields << fields << fields << fields << fields << std::endl;

    std::ofstream foutQuan("quantities.csv");
    for (int i = -128; i < 128; ++i)
        foutQuan << ";" << i;
    foutQuan << std::endl;

    for (int i = 1; i <= 5; ++i)
    {
        std::string path = "C:\\Users\\Asus\\Desktop\\samples-for-students\\";

        if (i < 10)
            path += "0";
        path += std::to_string(i);

        std::map<char, double> quantities;
        for (int l = -128; l < 128; ++l)
            quantities[l] = 0;

        std::string result = std::to_string(i) + ";" + std::to_string(countEntropyAndQuantities(path, quantities));

        foutQuan << i;
        for (const auto& q : quantities)
        {
            int integer = (int)q.second / 1;
            std::string decimal = std::to_string(q.second - integer);

            foutQuan << ";" << integer << "," << decimal.substr(2, decimal.length() - 2);
        }
        foutQuan << std::endl;

        for (int j = 3; j < 4; ++j)
        {
            uint64_t encodingElapsedTime = 0;
            uint64_t decodingElapsedTime = 0;

            for (int k = 0; k < ITER_NUM; k++)
            {
                auto startTime = gettime::now();
                algos[j]->encode(path, path + encodedPaths[j]);
                auto finishTime = gettime::now();
                encodingElapsedTime += std::chrono::duration_cast<ns>(finishTime - startTime).count();

                startTime = gettime::now();
                algos[j]->decode(path + encodedPaths[j], path + decodedPaths[j]);
                finishTime = gettime::now();
                decodingElapsedTime += std::chrono::duration_cast<ns>(finishTime - startTime).count();
            }

            encodingElapsedTime /= ITER_NUM;
            decodingElapsedTime /= ITER_NUM;

            int64_t originalSize = FileReaderWriter::getFileSize(path);
            int64_t encodedSize = FileReaderWriter::getFileSize(path + encodedPaths[j]);
            int64_t decodedSize = FileReaderWriter::getFileSize(path + decodedPaths[j]);

            std::cout << i << "\t" << j << std::endl;

            result += ";" + std::to_string((double)encodedSize / originalSize) +
                              ";" + std::to_string(encodingElapsedTime) +
                              ";" + std::to_string(decodingElapsedTime);
        }

        foutRes << result << std::endl;
    }

    foutRes.close();
    foutQuan.close();

    return 0;
}