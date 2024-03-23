// Don't change the ordering of include lib !!!!!!!!!!!!!!
// #include "robdd.h"
// #include "cudd.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <cctype>
#include "robdd.h"
#include "cudd.h"

ROBDD::ROBDD(DdManager* gbm) {
    this->gbm = gbm;
}

DdNode* ROBDD::constructBDD(std::vector<std::string>& products, std::unordered_map<char,int>& umap) {
    DdNode* exprBDD = Cudd_ReadLogicZero(gbm);
    Cudd_Ref(exprBDD);

    for (const std::string& product : products) {
        DdNode* termBDD = Cudd_ReadOne(gbm); 
        Cudd_Ref(termBDD);

        for (const char& ch : product) {
            if (std::isalpha(ch)) {
                if (std::islower(ch)) {
                    DdNode* varBDD = Cudd_bddIthVar(gbm, umap[ch]);
                    DdNode* tmpBDD = Cudd_bddAnd(gbm, termBDD, varBDD);
                    Cudd_Ref(tmpBDD);
                    Cudd_RecursiveDeref(gbm, termBDD);
                    termBDD = tmpBDD;
                }
                else {
                    DdNode* varBDD = Cudd_bddIthVar(gbm, umap[tolower(ch)]);
                    DdNode* negBDD = Cudd_Not(varBDD);
                    DdNode* tmpBDD = Cudd_bddAnd(gbm, termBDD, negBDD);
                    Cudd_Ref(tmpBDD);
                    Cudd_RecursiveDeref(gbm, termBDD);
                    termBDD = tmpBDD;
                }
            }
        }
        DdNode* tmpExprBDD = Cudd_bddOr(gbm, exprBDD, termBDD);
        Cudd_Ref(tmpExprBDD);
        Cudd_RecursiveDeref(gbm, exprBDD);
        Cudd_RecursiveDeref(gbm, termBDD);
        exprBDD = tmpExprBDD;
    }
    return exprBDD;
}

int ROBDD::reordering(DdNode* bdd, int* newOrder) {
    Cudd_ShuffleHeap(gbm, newOrder);
    DdNode *add = Cudd_BddToAdd(gbm, bdd); 
    Cudd_Ref(add);
    int nodeCountAdd = Cudd_DagSize(add);
    Cudd_RecursiveDeref(gbm, add);
    return nodeCountAdd;
}