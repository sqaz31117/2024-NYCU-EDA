# ifndef PARSER_H_
# define PARSER_H_

#include <string>
#include <vector>
#include <unordered_map>

class Parser {
public:
    Parser(std::string, std::string);
    std::vector<std::string> inputFileHandling();
    void outputFileHandling(const int);
    std::vector<std::string> getProductTerms(std::string);
    std::unordered_map<char,int> getHashTable(std::string);
private:
    std::string inputFilePath;
    std::string outputFilePath;
};

# endif