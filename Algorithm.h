#include "FileReaderWriter.h"

#ifndef KDZ_ALGORITHM_H
#define KDZ_ALGORITHM_H

class IAlgorithm
{
public:
    static const size_t CHAR_SIZE = FileReaderWriter::CHAR_SIZE;

    virtual void encode(const std::string& inputPath, const std::string& outputPath) {}
    virtual void decode(const std::string& inputPath, const std::string& outputPath) {}
};

#endif //KDZ_ALGORITHM_H
