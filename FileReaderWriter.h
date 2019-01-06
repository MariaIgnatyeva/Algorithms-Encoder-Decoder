#include <iostream>
#include <vector>
#include <fstream>
#include <tuple>

#ifndef KDZ_FILEREADER_H
#define KDZ_FILEREADER_H

class FileReaderWriter
{
public:
    static const size_t CHAR_SIZE = 256;

    static std::vector<char> getBytesFromFile(const std::string& path)
    {
        std::streampos size;
        std::vector<char> bytes;

        std::ifstream file(path, std::ios::binary | std::ios::ate);

        if (file)
        {
            size = file.tellg();
            bytes.resize(static_cast<size_t>(size));

            file.seekg(0, std::ios::beg);

            file.read(&bytes[0], static_cast<size_t>(size));
            file.close();
        }

        return bytes;
    }

    static void writeElementsToFile(const std::string& table, int8_t zeros, const std::string& text,
                                  const std::string& path)
    {
        std::ofstream fout(path, std::ios::binary);

        if (fout)
        {
            fout.write(table.c_str(), table.length());
            fout.write((char*)(&zeros), sizeof(char));
            fout.write(text.c_str(), text.length());

            fout.close();
        }
    }

    static void writeStringToFile(const std::string& str, const std::string& path)
    {
        std::ofstream fout(path, std::ios::binary);

        if (fout)
        {
            fout.write(str.c_str(), str.length());
            fout.close();
        }
    }

    static int64_t getFileSize(const std::string& path)
    {
        int64_t size = 0;

        std::ifstream fin(path, std::ios::ate);

        if (fin)
            size = fin.tellg();

        fin.close();

        return size;
    }
};

#endif //KDZ_FILEREADER_H
