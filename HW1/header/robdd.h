# ifndef ROBDD_H_
# define ROBDD_H_

#include "cudd.h"
#include <vector>
#include <unordered_map>

class ROBDD{
public:
    ROBDD(DdManager* gbm);
    DdNode* constructBDD(std::vector<std::string>&, std::unordered_map<char,int>&);
    int reordering(DdNode*, int*);
private:
    DdManager* gbm;
};

# endif