#include "parser.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

Parser::Parser(std::string inputFilePath, std::string outputFilePath) {
    this->inputFilePath = inputFilePath;
    this->outputFilePath = outputFilePath;
}

std::vector<std::string> Parser::inputFileHandling() {
    std::string line;
    std::vector<std::string> vec;
    std::ifstream inFile(this->inputFilePath);

    if (!inFile.is_open()) {
        std::cerr << "Can't open file" << this->inputFilePath << std::endl;
        return vec;
    }

    while (std::getline(inFile, line)) {
        vec.push_back(line);
    }
    
    inFile.close();
    return vec;
}

void Parser::outputFileHandling(const int ans) {
    std::ofstream outFile(this->outputFilePath);
    if (outFile.is_open()) {
        outFile << ans << '\n';
        outFile.close();
    } else {
        std::cerr << "Unable to open file" << std::endl; 
        return;
    }
}

std::vector<std::string> Parser::getProductTerms(std::string expr) {
    std::string product;
    std::vector<std::string> products;
    for (const char& ch : expr) {
        if (ch == '+' || ch == '.') {
            products.push_back(product);
            product.clear();
        }
        else {
            product += ch;
        }
    }
    return products;
}

std::unordered_map<char,int> Parser::getHashTable(std::string var) {
    std::unordered_map<char,int> umap;
    for (int i = 0; i < var.length()-1; i++) {
        umap[var[i]] = i;
    }
    return umap;
}