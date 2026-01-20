#include<iostream>
#include<bitset>
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

    const auto& files = inputFile.getFiles();
    if(!files.empty()){
        int lcount=0;
        const auto& bytes = files.front().data;
        for(int i=0; i< bytes.size();i++){
            if(++lcount == 16){
                lcount=0;
                cout <<endl;
            } 
            cout << bitset<4>(bytes[i])<<"\t";
        }
    }


    inputFile.saveCompressed("file/output", encodedFile, false);

    return 0;
}