#include "files.h"
#include <fstream>
#include <stdexcept>
#include <cstdint>

/* -----------------------------------------------------------
   Utility: check if a file is a .huff archive
----------------------------------------------------------- */
bool isHuffFile(const std::string& path)
{
    return path.size() >= 5 &&
           path.substr(path.size() - 5) == ".huff";
}

/* -----------------------------------------------------------
   Load a normal (non-.huff) file into memory
----------------------------------------------------------- */
void FileIO::loadFile(const std::string& path)
{
    FileData fd;

    // Find extension and last path separator
    const size_t extPos = path.find_last_of('.');
    const size_t namPos = path.find_last_of("/\\");

    if (extPos == std::string::npos || extPos == path.size() - 1)
        throw std::runtime_error("No valid file extension found");

    // Safely compute filename start
    const size_t nameStart =
        (namPos == std::string::npos) ? 0 : namPos + 1;

    // Extract filename and extension
    fd.fileName = path.substr(nameStart, extPos - nameStart);
    fd.fileExtension = path.substr(extPos + 1);

    // Open file in binary mode
    std::ifstream file(path, std::ios::binary);
    if (!file)
        throw std::runtime_error("Failed to open file");

    // Determine file size
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size <= 0)
        throw std::runtime_error("File is empty or unreadable");

    // Read entire file into memory
    fd.data.resize(static_cast<size_t>(size));

    if (!file.read(reinterpret_cast<char*>(fd.data.data()), size))
        throw std::runtime_error("Failed to read file data");

    files.push_back(std::move(fd));
}

/* -----------------------------------------------------------
   Access loaded raw files (read-only)
----------------------------------------------------------- */
const std::vector<FileData>& FileIO::getFiles() const
{
    return files;
}

/* -----------------------------------------------------------
   Save output:
   - if decoded  → write original files
   - if encoded  → write a .huff archive
----------------------------------------------------------- */
void FileIO::saveCompressed(
    const std::string& outputPath,
    const std::vector<HuffmanData>& data,
    bool isDecoded
) const
{
    /* -------------------------------------------------------
       Case 1: DECODED
       Write original files: name.extension
    ------------------------------------------------------- */
    if (isDecoded)
    {
        for (const auto& f : data)
        {
            const std::string outPath =
                outputPath + "/" + f.fileName + "." + f.fileExtension;

            std::ofstream output(outPath, std::ios::binary);
            if (!output)
                throw std::runtime_error("Failed to write file: " + outPath);

            // Write ONLY decoded bytes (NOT metadata, NOT structs)
            output.write(
                reinterpret_cast<const char*>(f.decodedData.data()),
                f.decodedData.size()
            );

            if (!output)
                throw std::runtime_error("Write failed: " + outPath);
        }
        return;
    }

    /* -------------------------------------------------------
       Case 2: ENCODED
       Write a single .huff archive
    ------------------------------------------------------- */
    std::ofstream output(outputPath + "/encoded.huff", std::ios::binary);
    if (!output)
        throw std::runtime_error("Failed to write encoded.huff");

    // Number of files in archive
    uint32_t fileCount = static_cast<uint32_t>(data.size());
    output.write(reinterpret_cast<const char*>(&fileCount), sizeof(fileCount));

    for (const auto& f : data)
    {
        uint32_t nameLen  = static_cast<uint32_t>(f.fileName.size());
        uint32_t extLen   = static_cast<uint32_t>(f.fileExtension.size());
        uint32_t tableSz  = static_cast<uint32_t>(f.encodeTable.size());
        uint32_t dataSz   = static_cast<uint32_t>(f.encodedData.size());

        // Write metadata
        output.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        output.write(f.fileName.data(), nameLen);

        output.write(reinterpret_cast<const char*>(&extLen), sizeof(extLen));
        output.write(f.fileExtension.data(), extLen);

        // Write Huffman table
        output.write(reinterpret_cast<const char*>(&tableSz), sizeof(tableSz));
        for(const auto& entry : f.encodeTable){
            output.write(reinterpret_cast<const char*>(&entry.first), sizeof(entry.first));
            output.write(reinterpret_cast<const char*>(&entry.second), sizeof(entry.second));
        }

        // Write encoded bitstream
        output.write(reinterpret_cast<const char*>(&dataSz), sizeof(dataSz));
        output.write(
            reinterpret_cast<const char*>(f.encodedData.data()),
            dataSz
        );
    }
}
