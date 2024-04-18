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
    int gain;
    list<int>::iterator gainIdx;
    unordered_map<int, list<int>::iterator> pos;
};

vector<pair<list<int>, list<int>>> nets;    // nets[idx].first store L part of nodes
                                            // nets[idx].right store R part of nodes

unordered_map<int, vector<int>> nodes;      // nodes[idx] store how many and which net connect to it
unordered_map<int, NodePosition> nodePositions;

vector<int> updateAnswer(int Nds) {
    // TODO
    vector<int> ans(Nds);
    for (int i = 0; i < Nds; ++i) {
        if (nodePositions[i].isLeft) ans[i] = 0;
        else ans[i] = 1;
    }
    return ans;
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

void updateGain(vector<bool>& freeCell, unordered_map<int, list<int>>& LPmax, 
unordered_map<int, list<int>>& RPmax, int& LPidx, int& RPidx, int& lb, int& rb, int moveID) {
    for (const int& net : nodes[moveID]) {
        for (const int& nf : nets[net].first) {
            if (freeCell[nf]) {
                int gn = calculateGain(nf);
                if (nodePositions[nf].isLeft) {
                    LPmax[nodePositions[nf].gain].erase(nodePositions[nf].gainIdx);
                    LPmax[gn].push_back(nf);
                    nodePositions[nf].gain = gn;
                    nodePositions[nf].gainIdx = --LPmax[gn].end();
                    LPidx = max(LPidx, gn);
                    lb = min(lb, gn);
                }
                else {
                    RPmax[nodePositions[nf].gain].erase(nodePositions[nf].gainIdx);
                    RPmax[gn].push_back(nf);
                    nodePositions[nf].gain = gn;
                    nodePositions[nf].gainIdx = --RPmax[gn].end();
                    RPidx = max(RPidx, gn);
                    rb = min(rb, gn);
                }
            }
        }
        for (const int& ns : nets[net].second) {
            if (freeCell[ns]) {
                int gn = calculateGain(ns);
                if (nodePositions[ns].isLeft) {
                    LPmax[nodePositions[ns].gain].erase(nodePositions[ns].gainIdx);
                    LPmax[gn].push_back(ns);
                    nodePositions[ns].gain = gn;
                    nodePositions[ns].gainIdx = --LPmax[gn].end();
                    LPidx = max(LPidx, gn);
                    lb = min(lb, gn);
                }
                else {
                    RPmax[nodePositions[ns].gain].erase(nodePositions[ns].gainIdx);
                    RPmax[gn].push_back(ns);
                    nodePositions[ns].gain = gn;
                    nodePositions[ns].gainIdx = --RPmax[gn].end();
                    RPidx = max(RPidx, gn);
                    rb = min(rb, gn);
                }
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
// moves -> {gain, nodeID}
void FMalgorithm(int bfL, int bfR, int Lpart, int Rpart, int Nds, vector<pair<int,int>>& moves) {
    auto start = high_resolution_clock::now();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    vector<bool> freeCell(Nds, true);
    bool canMove = true;
    int prevCut = INT_MAX;
    // vector<pair<int,int>> moves(Nds);

    unordered_map<int, list<int>> LPmax;
    unordered_map<int, list<int>> RPmax;
    int LPidx = INT_MIN;
    int RPidx = INT_MIN;
    int lb = INT_MAX;
    int rb = INT_MAX;

    for (int i = 0; i < Nds; i++) {
        int gain = calculateGain(i);
        nodePositions[i].gain = gain;
        if (nodePositions[i].isLeft) {
            LPmax[gain].push_back(i);
            nodePositions[i].gainIdx = --LPmax[gain].end();
            LPidx = max(LPidx, gain);
            lb = min(lb, gain);
        }
        else {
            RPmax[gain].push_back(i);
            nodePositions[i].gainIdx = --RPmax[gain].end();
            RPidx = max(RPidx, gain);
            rb = min(rb, gain);
        }
    }

    while (canMove) {
        canMove = false;
        // find largest gain in left part
        while (LPmax[LPidx].empty() && LPidx >= lb) --LPidx;
        // find largest gain in right part
        while (RPmax[RPidx].empty() && RPidx >= rb) --RPidx;
        
        if (LPidx < lb || RPidx < rb) break;

        if (LPidx >= RPidx) {
            if (LPidx == INT_MIN) break;
            int newLsize = Lpart+1;
            int newRsize = Rpart+1;
            if (bfL <= newRsize && newRsize <= bfR) {
                int moveID = LPmax[LPidx].front();
                // cout << "move " << moveID << " LPidx: " << LPidx << endl;
                moves.push_back({LPidx, moveID});
                moveNode(moveID, false);
                freeCell[moveID] = false;
                LPmax[LPidx].erase(nodePositions[moveID].gainIdx);
                updateGain(freeCell, LPmax, RPmax, LPidx, RPidx, lb, rb, moveID);
                canMove = true;
                ++Rpart;
                --Lpart;
            }
            else if (RPidx != INT_MIN && bfL <= newLsize && newLsize <= bfR) {
                int moveID = RPmax[RPidx].front();
                moves.push_back({RPidx, moveID});
                // cout << "move " << moveID << " RPidx: " << RPidx << endl;
                moveNode(moveID, true);
                freeCell[moveID] = false;
                RPmax[RPidx].erase(nodePositions[moveID].gainIdx);
                updateGain(freeCell, LPmax, RPmax, LPidx, RPidx, lb, rb, moveID);
                canMove = true;
                --Rpart;
                ++Lpart;
            }
        }
        else {
            int newLsize = Lpart+1;
            int newRsize = Rpart+1;
            if (bfL <= newLsize && newLsize <= bfR) {
                int moveID = RPmax[RPidx].front();
                // cout << "move " << moveID << " RPidx: " << RPidx << endl;
                moves.push_back({RPidx, moveID});
                moveNode(moveID, true);
                freeCell[moveID] = false;
                RPmax[RPidx].erase(nodePositions[moveID].gainIdx);
                updateGain(freeCell, LPmax, RPmax, LPidx, RPidx, lb, rb, moveID);
                canMove = true;
                --Rpart;
                ++Lpart;
            }
            else if (LPidx != INT_MIN && bfL <= newRsize && newRsize <= bfR) {
                int moveID = LPmax[LPidx].front();
                // cout << "move " << moveID << " LPidx: " << LPidx << endl;
                moves.push_back({LPidx, moveID});
                moveNode(moveID, false);
                freeCell[moveID] = false;
                LPmax[LPidx].erase(nodePositions[moveID].gainIdx);
                updateGain(freeCell, LPmax, RPmax, LPidx, RPidx, lb, rb, moveID);
                canMove = true;
                ++Rpart;
                --Lpart;
            }
        }
        stop = high_resolution_clock::now();
        duration = duration_cast<milliseconds>(stop - start);
        if (duration.count() / 1000 > 27) break;
    } // end while
    return;
}

int main(int argc, char *argv[]) {
    // auto start = high_resolution_clock::now();
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    std::ifstream inFile(argv[1]);
    // std::ifstream inFile("HW2\\lab2_student\\input2.txt.hgr");
    std::ofstream outFile(argv[2]);
    // std::ofstream outFile("output.txt");
    
    if (!inFile.is_open()) {
        std::cerr << "Can't open file\n";
        return 1;
    }
    if (!outFile.is_open()) {
        std::cerr << "Can't open output file\n";
        return 1;
    }

    string line;
    vector<bool> Partition;     // false -> right, true -> left
    int N = 0, M = 0, lines = 0;

    while(getline(inFile, line)) {
        istringstream iss(line);
        int word;
        if (N == 0 && M == 0) {
            iss >> N >> M;
            nets.resize(N);
            Partition.resize(M);
            continue;
        }
        
        while(iss >> word) {
            int nodeIdx = word-1;
            if (nodeIdx % 2) {   // right part
                nets[lines].second.push_back(nodeIdx);
                Partition[nodeIdx] = false;
                nodePositions[nodeIdx].isLeft = false;
                nodePositions[nodeIdx].pos[lines] = --nets[lines].second.end();
            }
            else {              // left part
                nets[lines].first.push_back(nodeIdx);
                Partition[nodeIdx] = true;
                nodePositions[nodeIdx].isLeft = true;
                nodePositions[nodeIdx].pos[lines] = --nets[lines].first.end();
            }
            nodes[nodeIdx].push_back(lines);
        }
        lines++;
    }
    inFile.close();

    int l = 0.45*M;
    int r = 0.55*M;
    vector<pair<int,int>> moves;
    
    if (M % 2) {
        FMalgorithm(l, r, (M/2)+1, M/2, M, moves);
    } else {
        FMalgorithm(l, r, M/2, M/2, M, moves);
    }
    
    int s = moves.size();
    if (s > 0) {
        int pntV = moves[0].first;
        int pnt = 0;
        vector<int> cumulate(s, 0);
        cumulate[0] = pntV;
        for (int i = 1; i < s; ++i) {
            cumulate[i] = moves[i].first + cumulate[i-1];
            if (cumulate[i] > pntV) {
                pnt = i;
                pntV = cumulate[i];
            }
        }

        for (int i = 0; i <= pnt; ++i) {
            int tmp = moves[i].second;
            Partition[tmp] = Partition[tmp] ^ true;
        }

        for (int i = 0; i < M; ++i) {
            outFile << Partition[i] << '\n';
        }
    }
    else {
        for (int i = 0; i < M; ++i) {
            outFile << Partition[i] << '\n';
        }
    }
    return 0;
}