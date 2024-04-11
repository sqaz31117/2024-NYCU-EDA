#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <utility>
#include <unordered_map>
#include <sstream>

using namespace std;

struct NodePosition {
    bool isLeft; // true if in L part, false if in R part
    list<int>::iterator position; // iterator to the node's position in the list
};

vector<pair<list<int>, list<int>>> nets;    // nets[idx].first store L part of nodes
                                            // nets[idx].right store R part of nodes

unordered_map<int, vector<int>> nodes;      // nodes[idx] store how many and which net connect to it
unordered_map<int, NodePosition> nodePositions;
vector<bool> freeCell;

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
    cout << FS << " " << TS << endl;
    return FS - TS;
}

void FMalgorithm(int bfL, int bfR) {
    // balance factor : 0.45 ~ 0.55
    // step1 : calculate Gi = FS(i) - TS(i)
    // step2 : move the largest gain node to another part if satisfy balance factor
    // step3 : lock the node which is moved to another part, and we don't need to calculate this node's Gi anymore
    // repeat step1 to step3 util all nodes are locked or cannot move anymore due to the balance factor
    bool canMove = true;
    
    while (canMove) {
        canMove = false;

    }
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
            freeCell.resize(M, false);
            continue;
        }
        
        while(iss >> word) {
            if ((word-1) % 2) {   // right part
                nets[lines].second.push_back(word-1);
                auto END = nets[lines].second.end();
                --END;
                nodePositions[word-1] = {false, END};
            }
            else {              // left part
                nets[lines].first.push_back(word-1);
                auto END = nets[lines].first.end();
                --END;
                nodePositions[word-1] = {true, END};
            }
            nodes[word-1].push_back(lines);
        }
        lines++;
    }

    for (int i = 0; i < 5; i++) {
        cout << "iteration " << i << endl;
        int G = calculateGain(i);
        cout << "G: " << G << endl;
    }
}