#include<iostream>
#include "files.h"
using namespace std;

int main()
{
    const string input = "file/input.jpg";

    FileIO inputFile = FileIO();

    inputFile.loadFile(input);

    vector<HuffmanData> encodedFile;
    for (const auto& f : inputFile.getFiles()){
        HuffmanData h;
        h.fileName = f.fileName;
        h.fileExtension = f.fileExtension;
        h.encodedData = f.data;
        h.decodedData = f.data;
        encodedFile.push_back(std::move(h));
    }

    inputFile.saveCompressed("file/output", encodedFile, false);

    return 0;
}