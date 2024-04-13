#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <utility>
#include <unordered_map>
#include <sstream>
#include <algorithm>

using namespace std;

struct NodePosition {
    bool isLeft; // true if in L part, false if in R part
};

vector<pair<list<int>, list<int>>> nets;    // nets[idx].first store L part of nodes
                                            // nets[idx].right store R part of nodes

unordered_map<int, vector<int>> nodes;      // nodes[idx] store how many and which net connect to it
unordered_map<int, NodePosition> nodePositions;

void updateAnswer() {
    // TODO
}

int calculateCutsize() {
    int cutSize = 0;
    for (auto P : nets) {
        if (P.first.size() != 0 && P.second.size() != 0) cutSize++;
    }
    return cutSize;
}

// calculate Gi = FS(i) - TS(i)
int calculateGain(int node) {
    int FS = 0, TS = 0;
    for (auto net : nodes[node]) {
        // FS
        if (nets[net].first.size() == 1 && nodePositions[node].isLeft) {
            FS++;
        }
        else if (nets[net].second.size() == 1 && !nodePositions[node].isLeft) {
            FS++;
        }

        // TS
        if (nets[net].first.size() == 0 && !nodePositions[node].isLeft) {
            TS++;
        }
        else if (nets[net].second.size() == 0 && nodePositions[node].isLeft) {
            TS++;
        }
    }
    return FS - TS;
}

void moveNode(int nodeToMove, bool toLeft) {
    NodePosition& pos = nodePositions[nodeToMove];
    if (pos.isLeft && !toLeft) {
        // node at left and move to right
        for (const int& net : nodes[nodeToMove]) {
            list<int>::iterator iter1 = std::find (nets[net].first.begin(), nets[net].first.end(), nodeToMove);
            if (iter1 != nets[net].first.end()) {
                // can move
                nets[net].first.erase(iter1);               // erase from left part
                nets[net].second.push_back(nodeToMove);     // move to right part
                nodePositions[nodeToMove].isLeft = false;   // move to right part
            }
            else {
                cout << "some error occur at L -> R\n";
            }
        }
    }
    else if (!pos.isLeft && toLeft) {
        for (const int& net : nodes[nodeToMove]) {
            list<int>::iterator iter2 = std::find (nets[net].second.begin(), nets[net].second.end(), nodeToMove);
            if (iter2 != nets[net].second.end()) {
                // can move
                nets[net].second.erase(iter2);               // erase from right part
                nets[net].first.push_back(nodeToMove);     // move to left part
                nodePositions[nodeToMove].isLeft = false;   // move to left part
            }
            else {
                cout << "some error occur at R -> L\n";
            }
        }
    }
}
// Algorithm
    // step1 : calculate Gi = FS(i) - TS(i) ------> Done
    // step2 : move the largest gain node to another part if satisfy balance factor
    // step3 : lock the node which is moved to another part, and we don't need to calculate this node's Gi anymore
    // repeat step1 to step3 util all nodes are locked or cannot move anymore due to the balance factor
// **** input parameter ***
// balance factor : 0.45 ~ 0.55
// bfL = balance factor Left bound
// bfR = balance factor Right bound
// Lpart = L part size
// Rpart = R part size
void FMalgorithm(int bfL, int bfR, int Lpart, int Rpart, int Nds) {
    vector<bool> freeCell(Nds, true);
    bool canMove = true;
    
    while (canMove) {
        canMove = false;
        vector<pair<int,int>> Lgain;  Lgain.clear();
        vector<pair<int,int>> Rgain;  Rgain.clear();

        // step1
        for (int i = 0; i < Nds; i++) {
            if (freeCell[i]) {
                int gain = calculateGain(i);
                if (nodePositions[i].isLeft) {
                    Lgain.push_back({gain, i});
                }
                else {
                    Rgain.push_back({gain, i});
                }
            }
        }
        sort(Lgain.rbegin(), Lgain.rend());
        sort(Rgain.rbegin(), Rgain.rend());

        // step2: check balance factor and chooose the best one to move
        int Lsize = Lgain.size();
        int Rsize = Rgain.size();
        if (Lsize != 0 && Rsize != 0) {
            if (Lgain[0].first >= Rgain[0].first) {
                if (bfL <= Rpart+1 &&  Rpart+1 <= bfR) {
                    int moveID = Lgain[0].second;
                    moveNode(moveID, false);
                    canMove = true;
                    freeCell[moveID] = false;
                    ++Rpart;
                    --Lpart;
                }
                else if (bfL <= Lpart+1 && Lpart+1 <= bfR) {
                    int moveID = Rgain[0].second;
                    moveNode(moveID, true);
                    canMove = true;
                    freeCell[moveID] = false;
                    ++Lpart;
                    --Rpart;
                }
            }
            else {
                if (bfL <= Lpart+1 && Lpart+1 <= bfR) {
                    int moveID = Rgain[0].second;
                    moveNode(moveID, true);
                    canMove = true;
                    freeCell[moveID] = false;
                    ++Lpart;
                    --Rpart;
                }
                else if (bfL <= Rpart+1 &&  Rpart+1 <= bfR) {
                    int moveID = Lgain[0].second;
                    moveNode(moveID, false);
                    canMove = true;
                    freeCell[moveID] = false;
                    ++Rpart;
                    --Lpart;
                }
            }
        }
        else if (Lsize != 0 && Rsize == 0) {
            if (bfL <= Rpart+1 &&  Rpart+1 <= bfR) {
                int moveID = Lgain[0].second;
                moveNode(moveID, false);
                canMove = true;
                freeCell[moveID] = false;
                ++Rpart;
                --Lpart;
            }
        }
        else if (Lsize == 0 && Rsize != 0) {
            if (bfL <= Lpart+1 && Lpart+1 <= bfR) {
                int moveID = Rgain[0].second;
                moveNode(moveID, true);
                canMove = true;
                freeCell[moveID] = false;
                ++Lpart;
                --Rpart;
            }
        }

    } // end while
}

int main(int argc, char *argv[]) {
    std::ifstream inFile(argv[1]);
    if (!inFile.is_open()) {
        std::cerr << "Can't open file\n";
        return 1;
    }

    string line;
    int N = 0, M = 0, mid = 0, lines = 0;

    while(getline(inFile, line)) {
        istringstream iss(line);
        int word;
        if (N == 0 && M == 0) {
            iss >> N >> M;
            mid = M / 2;
            nets.resize(N);
            continue;
        }
        
        while(iss >> word) {
            int nodeIdx = word-1;
            if (nodeIdx % 2) {   // right part
                nets[lines].second.push_back(nodeIdx);
                nodePositions[nodeIdx].isLeft = false;
            }
            else {              // left part
                nets[lines].first.push_back(nodeIdx);
                nodePositions[nodeIdx].isLeft = true;
            }
            nodes[nodeIdx].push_back(lines);
        }
        lines++;
    }
    FMalgorithm(3, 5, 4, 4, 8);
}