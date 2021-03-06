#ifndef __MC_TREE__
#define __MC_TREE__

#include "util.h"
#include "constants.h"
#include "board.h"
#include <queue>
#include <ctime>
#include <utility>
#include <cstring>

namespace zc {

class MCTree {
public:
    MCTree(int playerID_, int N, int M, int noX, int noY);
    // ~MCTree();

    std::pair<int, int> UCTSearch();
    void changeRootTo(std::pair<int, int> decision);
    std::pair<int, int> getLastDecision() const {
        return std::make_pair((int)last.x, (int)last.y);
    }

    static void timerStart() {
        startTime = clock();
    }

    static bool timeout() {
        clock_t endTime = clock();
        return endTime - startTime > MC_TIME_LIMIT;
    }

private:
    int newNode(int fa);
    void delNode(int x);
    void deleteSubTree(int x);
    void backTrack(int x, int value);
    int treePolicy(int x, Board* board);
    int defaultPolicy(int x, Board* board);
    void outputTree(int x);
    int getTreeSize();
    struct Node {
        int child[MAX_WIDTH];
        int latest;
        int cnt, value;
        int fa;
        void refresh(int fa_) {
            memset(child, 0, sizeof(child));
            latest = -2;
            cnt = value = 0;
            fa = fa_;
        }
    };

    struct NodePool {
        int q[MAX_TREE_NODE + 10];
        int h, w;
        NodePool() : h(0), w(0) {}
        void push(int x) {
            h++;
            if (h > MAX_TREE_NODE)
                h = 1;
            q[h] = x;
        }

        int pop() {
            w++;
            if (w > MAX_TREE_NODE)
                w = 1;
            return q[w];
        }

        bool empty() {
            return h == w;
        }
    };

    Node tr[MAX_TREE_NODE];
    bool playerID;
    static time_t startTime;
    int nodeCnt;
    // int n, m, noX, noY;
    Board board;
    int n, m;
    NodePool nodePool;
    Point last;
    int root, toPlay;
};

}

#endif //__MC_TREE__