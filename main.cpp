#include<iostream>
#include "files.h"
using namespace std;

int main()
{
    const string input = "file/input.jpg";

    auto file = File::read(input);

    File::write("file/output.bin", file);

    return 0;
}