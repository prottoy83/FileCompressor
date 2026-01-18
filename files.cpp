#include "files.h"
#include<fstream>
#include <stdexcept>

std::vector<uint8_t> File::read(const std::string& path){
    std::ifstream file(path, std::ios::binary);
    if(!file){
        throw std::runtime_error("Error opening file: "+ path);
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buff(static_cast<size_t>(size));
    file.read(reinterpret_cast<char*>(buff.data()), size);
    if(!file){
        throw std::runtime_error("Error Reading file!");
    }

    return buff;
}

void File::write(const std::string& path, const std::vector<uint8_t>& data){
    std::ofstream outfile(path, std::ios::binary);
    outfile.write(reinterpret_cast<const char*>(data.data()), data.size());
}