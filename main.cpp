#include<iostream>
#include<fstream>
#include<vector>
#include<cstdint>

using namespace std;

int main()
{
    const string input = "file/input.jpg";

    //File input
    ifstream file(input, ios::binary);
    if(!file){
        cerr << "Error on reading the file" <<endl;
        return -1;
    }

    //File size calculation
    file.seekg(0, ios::end);
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    //Store data in buffer
    vector<uint8_t> buffer(static_cast<size_t>(size));
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    //output to output.bin file
    ofstream outfile("file/output.bin", ios::binary);
    outfile.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());

    return 0;
}