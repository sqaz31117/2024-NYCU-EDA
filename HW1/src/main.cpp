#include <stdio.h>
#include <stdlib.h>
#include "cudd.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <math.h>
#include <algorithm>
#include "parser.h"
#include "robdd.h"

/**
 * Writes a dot file representing the argument DDs
 * @param the node object
 */
void write_dd (DdManager *gbm, DdNode *dd, char* filename)
{
    FILE *outfile; // output file pointer for .dot file
    outfile = fopen(filename,"w");
    DdNode **ddnodearray = (DdNode**)malloc(sizeof(DdNode*)); // initialize the function array
    ddnodearray[0] = dd;
    Cudd_DumpDot(gbm, 1, ddnodearray, NULL, NULL, outfile); // dump the function to .dot file
    free(ddnodearray);
    fclose (outfile); // close the file */
}

int main (int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input file>" << " <output file>" << std::endl;
        return 1;
    }

    Parser* parser = new Parser(argv[1], argv[2]);
    std::vector<std::string> lines = parser->inputFileHandling();
    std::vector<std::string> products = parser->getProductTerms(lines[0]);
    std::unordered_map<char,int> hashTable = parser->getHashTable(lines[1]);
    
    DdManager* gbm = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    ROBDD* robdd = new ROBDD(gbm);
    DdNode* exprBDD = robdd->constructBDD(products, hashTable);

    std::vector<int> ans;
    int len = hashTable.size();

    for (int i = 1; i < lines.size(); i++) {
        int order[len] = {-1};
        for (int j = 0; j < len; j++) {
            order[j] = hashTable[lines[i][j]];
        }
        ans.push_back(robdd->reordering(exprBDD, order));
    }

    std::sort(ans.begin(), ans.end());
    parser->outputFileHandling(ans[0]);
    Cudd_Quit(gbm);
    return 0; 
}