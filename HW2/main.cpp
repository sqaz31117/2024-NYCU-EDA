#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <utility>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <climits>
#include <chrono>
#include <random>

using namespace std;
using namespace std::chrono;

struct NodePosition {
    bool isLeft; // true if in L part, false if in R part
    unordered_map<int, list<int>::iterator> pos;
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
    NodePosition& Pos = nodePositions[nodeToMove];
    if (Pos.isLeft && !toLeft) {
        nodePositions[nodeToMove].isLeft = false;
        for (const int net : nodes[nodeToMove]) {
            // list<int>::iterator iter1 = std::find (nets[net].first.begin(), nets[net].first.end(), nodeToMove);
            list<int>::iterator iter1 = nodePositions[nodeToMove].pos[net];
            if (iter1 != nets[net].first.end()) {
                // can move
                nets[net].first.erase(iter1);               // erase from left part
                nets[net].second.push_back(nodeToMove);     // move to right part
                nodePositions[nodeToMove].pos[net] = --nets[net].second.end();
            }
            else {
                cout << "some error occur at L -> R\n";
            }
        }
    }
    else if (!Pos.isLeft && toLeft) {
        nodePositions[nodeToMove].isLeft = true;   // move to left part
        for (const int net : nodes[nodeToMove]) {
            list<int>::iterator iter2 = nodePositions[nodeToMove].pos[net];
            if (iter2 != nets[net].second.end()) {
                // can move
                nets[net].second.erase(iter2);               // erase from right part
                nets[net].first.push_back(nodeToMove);     // move to left part
                nodePositions[nodeToMove].pos[net] = --nets[net].first.end();
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
void FMalgorithm(int bfL, int bfR, int Lpart, int Rpart, int Nds, const string name) {
    auto start = high_resolution_clock::now();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    vector<bool> freeCell(Nds, true);
    bool canMove = true;
    int cnt = 0;
    std::ofstream outFile(name);
    while (canMove) {
        canMove = false;
        outFile << calculateCutsize() << '\n';
        unordered_map<int, vector<int>> LPmax; LPmax.clear();
        unordered_map<int, vector<int>> RPmax; RPmax.clear();
        int LPidx = INT_MIN;
        int RPidx = INT_MIN;

        // TODO
        for (int i = 0; i < Nds; i++) {
            if (freeCell[i]) {
                int gain = calculateGain(i);
                if (nodePositions[i].isLeft) {
                    LPmax[gain].push_back(i);
                    LPidx = max(LPidx, gain);
                }
                else {
                    RPmax[gain].push_back(i);
                    RPidx = max(RPidx, gain);
                }
            }
        }

        // step2: check balance factor and chooose the best one to move
        // int Lsize = Lgain.size();
        // int Rsize = Rgain.size();
        if (LPidx >= RPidx) {
            if (LPidx == INT_MIN) break;
            int newLsize = Lpart+1;
            int newRsize = Rpart+1;
            if (bfL <= newRsize && newRsize <= bfR) {
                int moveID = LPmax[LPidx][0];
                moveNode(moveID, false);
                canMove = true;
                freeCell[moveID] = false;
                ++Rpart;
                --Lpart;
                cnt++;
            }
            else if (RPidx != INT_MIN && bfL <= newLsize && newLsize <= bfR) {
                int moveID = RPmax[RPidx][0];
                moveNode(moveID, true);
                canMove = true;
                freeCell[moveID] = false;
                --Rpart;
                ++Lpart;
                cnt++;
            }
        }
        else {
            int newLsize = Lpart+1;
            int newRsize = Rpart+1;
            if (bfL <= newLsize && newLsize <= bfR) {
                int moveID = RPmax[RPidx][0];
                moveNode(moveID, true);
                canMove = true;
                freeCell[moveID] = false;
                --Rpart;
                ++Lpart;
                cnt++;
            }
            else if (LPidx != INT_MIN && bfL <= newRsize && newRsize <= bfR) {
                int moveID = LPmax[LPidx][0];
                moveNode(moveID, false);
                canMove = true;
                freeCell[moveID] = false;
                ++Rpart;
                --Lpart;
                cnt++;
            }
        }
        stop = high_resolution_clock::now();
        duration = duration_cast<milliseconds>(stop - start);
        if (duration.count() / 1000 > 28) break;
    } // end while
    // cout << "Time taken by function: " << duration.count() << " milliseconds" << endl;
}

int main(int argc, char *argv[]) {
    // auto start = high_resolution_clock::now();
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    std::ifstream inFile(argv[1]);

    if (!inFile.is_open()) {
        std::cerr << "Can't open file\n";
        return 1;
    }

    string line;
    int N = 0, M = 0, lines = 0;

    while(getline(inFile, line)) {
        istringstream iss(line);
        int word;
        if (N == 0 && M == 0) {
            iss >> N >> M;
            nets.resize(N);
            continue;
        }
        
        while(iss >> word) {
            int nodeIdx = word-1;
            if (nodeIdx % 2) {   // right part
                nets[lines].second.push_back(nodeIdx);
                nodePositions[nodeIdx].isLeft = false;
                nodePositions[nodeIdx].pos[lines] = --nets[lines].second.end();
            }
            else {              // left part
                nets[lines].first.push_back(nodeIdx);
                nodePositions[nodeIdx].isLeft = true;
                nodePositions[nodeIdx].pos[lines] = --nets[lines].first.end();
            }
            nodes[nodeIdx].push_back(lines);
        }
        lines++;
    }

    int l = 0.45*M;
    int r = 0.55*M;
    if (M % 2) {
        FMalgorithm(l, r, (M/2)+1, M/2, M, argv[2]);
    } else {
        FMalgorithm(l, r, M/2, M/2, M, argv[2]);
    }
}