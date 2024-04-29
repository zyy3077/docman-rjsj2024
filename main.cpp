#include <fstream>
#include <iostream>
#include <vector>

#include "utils.hpp"
#include "citation.h"

std::vector<Citation*> loadCitations(const std::string& filename) {
    // FIXME: load citations from file
    std::vector<Citation*> citations;
    std::ifstream file(filename);
    std::string line;
    while(std::getline(file, line)){
        citations.push_back(new Citation(line));
    }
    return citations;
}

std::string readFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

std::string readFromStdin() {
    std::string content((std::istreambuf_iterator<char>(std::cin)), std::istreambuf_iterator<char>());
    return content;
}

int main(int argc, char** argv) {
    // "docman", "-c", "citations.json", "input.txt"

    // FIXME: read all input to the string, and process citations in the input text
    //auto input = readFromFile(argv[argc -1]);
    // ...
    std::string citationPath;
    std::string outputPath;
    std::string inputPath;
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg == "-c" && i + 1 < argc) {
            citationPath = argv[++i];
        } else if (arg == "-o" && i + 1 < argc) {
            outputPath = argv[++i];
        } else {
            inputPath = arg;
        }
    }

    auto citations = loadCitations(citationPath);
    std::vector<Citation*> printedCitations{};

    std::string input;
    if (inputPath == "-") {
        input = readFromStdin();
    } else {
        input = readFromFile(inputPath);
    }

    std::ostream* output;
    if (!outputPath.empty()) {
        output = new std::ofstream(outputPath);
    } else {
        output = &std::cout;
    }

    *output << input;  // print the paragraph first
    *output << "\nReferences:\n";
    
    for (auto c : printedCitations) {
        // FIXME: print citation

    }

    for (auto c : citations) {
        delete c;
    }
}
