# File Compressor

A from-scratch binary-safe file compressor written in modern C++, designed as a learning-focused but technically correct implementation of lossless compression.
The project emphasizes bit-level correctness, file format design, and clean decompression, rather than relying on existing compression libraries.


## Features

- Reads any file type as raw binary data
- Byte-level Huffman coding (256-symbol alphabet)
- Custom bitstream writer/reader
- Deterministic decompression
- Portable, standard C++ (no external dependencies)
- Clear on-disk file format with metadata
## Run Locally

Clone the project

```bash
  git clone https://link-to-project
```

Go to the project directory

```bash
  cd FileCompressor
```

Install dependencies
- Cmake
- C++17 or newer (GCC/CLANG/MSVC)

Build

```bash
  cd Build
```
Cmake Generate
```bash
    cmake ..
```
ninja Build
```bash
    make
```
Run the executeable

