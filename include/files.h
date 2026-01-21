#ifndef FILE_IO_H
#define FILE_IO_H

#include <vector>
#include <cstdint>
#include <string>
#include <map>

/* -----------------------------------------------------------
   Raw file loaded from disk (before Huffman encoding)
----------------------------------------------------------- */
struct FileData {
    std::string fileName;              // File name without extension
    std::string fileExtension;         // File extension (png, jpg, txt, etc.)
    std::vector<uint8_t> data;         // Raw file bytes
};

/* -----------------------------------------------------------
   Huffman-processed file
----------------------------------------------------------- */
struct HuffmanData {
    std::string fileName;              // Original file name
    std::string fileExtension;         // Original file extension
    std::map<uint8_t, int> encodeTable;  // Serialized Huffman table
    std::vector<uint8_t> encodedData;  // Huffman-compressed bitstream
    std::vector<uint8_t> decodedData;  // Decoded original file bytes
};

/* -----------------------------------------------------------
   File I/O manager
----------------------------------------------------------- */
class FileIO {
private:
    std::vector<FileData> files;        // Loaded raw files

public:
    FileIO() = default;

    // Load a normal (non-.huff) file from disk
    void loadFile(const std::string& path);

    // Access loaded raw files (read-only)
    const std::vector<FileData>& getFiles() const;

    // Save output:
    // - isDecoded = true  → write original files
    // - isDecoded = false → write a .huff archive
    void saveCompressed(
        const std::string& outputPath,
        const std::vector<HuffmanData>& data,
        bool isDecoded
    ) const;
};

#endif // FILE_IO_H
