#include<iostream>
#include<bitset>
#include "files.h"
#include <queue>
#include <memory>
#include <optional>
#include <functional>
#include <fstream>
#include <stdexcept>
#include <limits>

using namespace std;

struct HuffNode {
    int freq;
    std::optional<uint8_t> value; 
    std::shared_ptr<HuffNode> left;
    std::shared_ptr<HuffNode> right;
};

static std::shared_ptr<HuffNode> buildTree(const std::map<uint8_t, int>& freq)
{
    struct Cmp {
        bool operator()(const std::shared_ptr<HuffNode>& a, const std::shared_ptr<HuffNode>& b) const {
            return a->freq > b->freq; // min-heap
        }
    };

    std::priority_queue<std::shared_ptr<HuffNode>, std::vector<std::shared_ptr<HuffNode>>, Cmp> pq;

    for (const auto& [byte, f] : freq) {
        auto node = std::make_shared<HuffNode>();
        node->freq = f;
        node->value = byte;
        pq.push(node);
    }

    if (pq.empty()) return nullptr;
    if (pq.size() == 1) return pq.top();

    while (pq.size() > 1) {
        auto a = pq.top(); pq.pop();
        auto b = pq.top(); pq.pop();

        auto parent = std::make_shared<HuffNode>();
        parent->freq = a->freq + b->freq;
        parent->left = a;
        parent->right = b;
        pq.push(parent);
    }

    return pq.top();
}


static std::map<uint8_t, std::string> buildCodes(const std::map<uint8_t, int>& freq)
{
    auto root = buildTree(freq);

    if (!root) return {};
    if (!root->left && !root->right && root->value.has_value()) {
        return { { *root->value, "0" } };
    }

    std::map<uint8_t, std::string> codes;
    std::string path;

    std::function<void(const std::shared_ptr<HuffNode>&, const std::string&)> dfs = [&](const std::shared_ptr<HuffNode>& node, const std::string& cur) {
        if (!node) return;
        if (node->value.has_value()) {
            codes[*node->value] = cur;
            return;
        }
        dfs(node->left, cur + "0");
        dfs(node->right, cur + "1");
    };

    dfs(root, "");
    return codes;
}

static std::vector<uint8_t> encodeBytes(const std::vector<uint8_t>& data, const std::map<uint8_t, std::string>& codes)
{
    std::vector<uint8_t> out;
    uint32_t bitCount = 0;

    uint8_t current = 0;
    int fill = 0; // bits filled in current byte

    auto flushBit = [&](int bit) {
        current = static_cast<uint8_t>(current | (bit & 1) << (7 - fill));
        fill++;
        bitCount++;
        if (fill == 8) {
            out.push_back(current);
            current = 0;
            fill = 0;
        }
    };

    for (uint8_t b : data) {
        const std::string& code = codes.at(b);
        for (char c : code) {
            flushBit(c == '1');
        }
    }

    if (fill > 0) {
        out.push_back(current); // last byte padded with zeros
    }

    std::vector<uint8_t> withHeader;
    withHeader.reserve(out.size() + sizeof(uint32_t));
    for (int i = 0; i < 4; ++i) {
        withHeader.push_back(static_cast<uint8_t>((bitCount >> (8 * i)) & 0xFF));
    }
    withHeader.insert(withHeader.end(), out.begin(), out.end());
    return withHeader;
}

static std::vector<uint8_t> decodeBytes(const std::vector<uint8_t>& encoded, const std::map<uint8_t, int>& freq)
{
    if (encoded.size() < sizeof(uint32_t)) return {};

    uint32_t bitCount = 0;
    for (int i = 0; i < 4; ++i) {
        bitCount |= static_cast<uint32_t>(encoded[i]) << (8 * i);
    }

    auto root = buildTree(freq);
    if (!root) return {};

    if (!root->left && !root->right && root->value.has_value()) {
        std::vector<uint8_t> out;
        int total = 0;
        for (const auto& [_, f] : freq) total += f;
        out.assign(static_cast<size_t>(total), *root->value);
        return out;
    }

    std::vector<uint8_t> out;
    const size_t byteCount = encoded.size() - sizeof(uint32_t);
    const uint8_t* bitData = encoded.data() + sizeof(uint32_t);

    auto node = root;
    uint32_t bitsRead = 0;
    for (size_t i = 0; i < byteCount && bitsRead < bitCount; ++i) {
        uint8_t byte = bitData[i];
        for (int bit = 7; bit >= 0 && bitsRead < bitCount; --bit) {
            const bool one = (byte >> bit) & 1;
            node = one ? node->right : node->left;
            if (node && node->value.has_value()) {
                out.push_back(*node->value);
                node = root;
            }
            bitsRead++;
        }
    }

    return out;
}

static std::vector<HuffmanData> loadEncodedArchive(const std::string& path)
{
    std::ifstream input(path, std::ios::binary);
    if (!input) throw std::runtime_error("Failed to open encoded archive: " + path);

    uint32_t fileCount = 0;
    input.read(reinterpret_cast<char*>(&fileCount), sizeof(fileCount));
    if (!input) throw std::runtime_error("Failed to read archive header: " + path);

    std::vector<HuffmanData> files;
    files.reserve(fileCount);

    for (uint32_t i = 0; i < fileCount; ++i) {
        uint32_t nameLen = 0, extLen = 0, tableSz = 0, dataSz = 0;

        input.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string name(nameLen, '\0');
        input.read(name.data(), nameLen);

        input.read(reinterpret_cast<char*>(&extLen), sizeof(extLen));
        std::string ext(extLen, '\0');
        input.read(ext.data(), extLen);

        input.read(reinterpret_cast<char*>(&tableSz), sizeof(tableSz));
        std::map<uint8_t, int> table;
        for (uint32_t t = 0; t < tableSz; ++t) {
            uint8_t key = 0; int val = 0;
            input.read(reinterpret_cast<char*>(&key), sizeof(key));
            input.read(reinterpret_cast<char*>(&val), sizeof(val));
            table[key] = val;
        }

        input.read(reinterpret_cast<char*>(&dataSz), sizeof(dataSz));
        std::vector<uint8_t> encoded(dataSz);
        if (dataSz > 0) {
            input.read(reinterpret_cast<char*>(encoded.data()), dataSz);
        }

        if (!input) throw std::runtime_error("Corrupt archive: " + path);

        HuffmanData h;
        h.fileName = std::move(name);
        h.fileExtension = std::move(ext);
        h.encodeTable = std::move(table);
        h.encodedData = std::move(encoded);
        h.decodedData = decodeBytes(h.encodedData, h.encodeTable);
        files.push_back(std::move(h));
    }

    return files;
}

int main()
{
    const std::string inputDir = "file/";
    const std::string encodedDir = "file/encoded";
    const std::string decodedDir = "file/output";

    cout << "Choose an action:\n1) Encode a file\n2) Decode a .huff file\n> ";
    int choice = 0;
    if (!(cin >> choice)) {
        cerr << "Invalid choice" << endl;
        return 1;
    }
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    cout << "Enter the file name (with extension): ";
    std::string fileName;
    std::getline(cin, fileName);

    try {
        if (choice == 1) {
            FileIO inputFile;
            inputFile.loadFile(inputDir + fileName);

            map<uint8_t, int> HuffmanTable;
            for (const auto& f : inputFile.getFiles()) {
                for (uint8_t b : f.data) HuffmanTable[b]++;
            }

            const auto codes = buildCodes(HuffmanTable);

            vector<HuffmanData> encodedFile;
            for (const auto& f : inputFile.getFiles()){
                HuffmanData h;
                h.fileName = f.fileName;
                h.fileExtension = f.fileExtension;
                h.encodeTable = HuffmanTable;
                h.encodedData = encodeBytes(f.data, codes);
                h.decodedData = f.data;
                encodedFile.push_back(std::move(h));
            }

            cout << "Writing encoded archive to " << encodedDir << endl;
            inputFile.saveCompressed(encodedDir, encodedFile, false);
            cout << "Done." << endl;
        }
        else if (choice == 2) {
            const std::string path = encodedDir + "/" + fileName;
            auto decoded = loadEncodedArchive(path);

            FileIO out;
            cout << "Decoding to " << decodedDir << endl;
            out.saveCompressed(decodedDir, decoded, true);
            cout << "Done." << endl;
        }
        else {
            cerr << "Unsupported option." << endl;
            return 1;
        }
    } catch (const std::exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        return 1;
    }

    return 0;
}