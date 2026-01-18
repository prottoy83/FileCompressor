#ifndef BIN_FILE
#define BIN_FILE

#include<vector>
#include<cstdint>
#include<string>

class File{
    public:
        static std::vector<uint8_t> read(const std::string& path);
        static void write(const std::string& path, const std::vector<uint8_t>& data);
};

#endif