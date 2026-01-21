#include<iostream>
#include<bitset>
#include "files.h"
#include <map>
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
    map<uint8_t, int> HuffmanTable;

    const auto& files = inputFile.getFiles();
    if(!files.empty()){
        const auto& bytes = files.front().data;
        for(int i=0; i< bytes.size();i++){
            HuffmanTable[bytes[i]]++;
        } 
    }

    cout <<"HUFFMAN ENCODED TABLE" <<endl;
    cout <<"DATA\tFrequency" <<endl;
    for(int i=0;i<256;i++){
        if(HuffmanTable[i] > 0){
            cout << static_cast<char>(i) <<"\t"<< HuffmanTable[i]<<endl;
        }
    }


    inputFile.saveCompressed("file/output", encodedFile, false);

    return 0;
}