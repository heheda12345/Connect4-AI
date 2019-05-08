#include "MCTree.h"
#include <cstring>
#include <cmath>
#include <cstdio>
#include "constants.h"
#include <conio.h>
namespace zc {

MCTree::MCTree(int playerID_, int N, int M, int noX, int noY):
    playerID(playerID_), nodeCnt(1), board(N, M, noX, noY), n(N), m(M), nodePool(), root(1), toPlay(0) {
        tr[1].refresh(-1);
        tr[1].latest = -1;
    }

time_t MCTree::startTime = 0;

int MCTree::newNode(int fa) {
    if (nodeCnt+1 < MAX_TREE_NODE) {
        nodeCnt++;
        tr[nodeCnt].refresh(fa);
        // _cprintf("new node %d\n", nodeCnt);
        if (nodeCnt == MAX_TREE_NODE - 1) {
            _cprintf("use all nodes, getting other from pool\n");
        }
        return nodeCnt;
    }
    if (nodePool.empty())
        return 0;
    int x = nodePool.pop();
    for (int i = 0; i < n; i++)
        if (tr[x].child[i])
            nodePool.push(tr[x].child[i]);
    tr[x].refresh(fa);
    // _cprintf("new Node from recycle %d\n", x);
    return x;
}

void MCTree::delNode(int x) {
    memset(tr[x].child, 0, sizeof(tr[x].child));
    nodePool.push(x);
}

void MCTree::deleteSubTree(int x) { 
    // _cprintf("delete subtree %d\n", x);
    // for (int i=0; i<m; i++)
    //    if (tr[x].child[i])
    //        deleteSubTree(tr[x].child[i]);
    // delNode(x);
    nodePool.push(x);
}

void MCTree::changeRootTo(std::pair<int, int> decision) {
	/* decision is in TA's format*/
	//_cprintf("decision: %d %d\n", decision.first, decision.second);
    toPlay ^= 1;
    int col = decision.first;
    for (int i=0; i<n; i++)
        if (i != col && tr[root].child[i])
            deleteSubTree(tr[root].child[i]);

    if (!tr[root].child[col]) {
        tr[root].refresh(-1); // use the previous root as new root
    } else {
        int preRoot = root;
        root = tr[root].child[col];
        tr[root].fa = -1;
        delNode(preRoot);
    }
	board.put(col);
    // board.output();
}

void MCTree::backTrack(int x, int value) {
    do {
        tr[x].value += value;
        tr[x].cnt++;
        x = tr[x].fa;
        value = -value;
        // _cprintf("backTrack %d\n", x);
    } while (x != root);
    tr[x].value += value;
    tr[x].cnt++;
}

int MCTree::getTreeSize() {
    std::queue<int> q;
    q.push(root);
    int cnt = 0;
    while (!q.empty()) {
        int x = q.front();
        q.pop();
        cnt++;
        for (int i=0; i<n; i++)
            if (tr[x].child[i]) {
                q.push(tr[x].child[i]);
            }
    }
    return cnt;
}


bool MCTree::treePolicy(int* value, int* finalState) {
    int x = root;
    Board board = this->board;
    while (1) {
        float mx = -INF; // TODO: 搞大
        int decision = -1;
        if (board.isEnd()) {
            //_cprintf("game end!\n");
            *value = board.evaluate();
            break;
        }
        for (int i=0; i<n; i++) // TODO: 一个合理的枚举顺序
            if (board.canPut(i)) {
                int y = tr[x].child[i];
                float confident;
                if (!y) {
                    confident = UCT_UNREACHED_CONFIDENT + board.prefer(i);
                } else {
                    // _cprintf("%d: [x] %d %d [y] %d %d\n", y, tr[x].cnt, tr[x].value, tr[y].cnt, tr[y].value);
                    confident = tr[y].value*1.0/tr[y].cnt + UCT_C * sqrt(2*log(tr[x].cnt*1.0/tr[y].cnt)) + board.prefer(i);
                }
                if (confident > mx) {
                    mx = confident;
                    decision = i;
                }
                //_cprintf("confident %d: %f\n", i, confident);
            }
		// _cprintf("decision: %d\n", decision);
        if (decision == -1) {
            _cprintf("decision = -1\n");
            for (int i = 0; i < n; i++) {
                if (tr[x].child[i]) {
                    int y = tr[x].child[i];
                    _cprintf("%d(%d): [x] %d %d [y] %d %d\n", y, i, tr[x].cnt, tr[x].value, tr[y].cnt, tr[y].value);
                }
                else {
                    _cprintf("no (%d): %d\n", i, tr[x].child[i]);
                }
            }
            system("pause");
        }
        board.put(decision);
		//board.output();
        int y = tr[x].child[decision];
        if (!y) {
            y = newNode(x);
            if (!y) {
                _cprintf("use all node\n");
                int cnt = getTreeSize();
                _cprintf("tree size %d\n", cnt);
                system("pause"); // TODO: a robust stategy
            }
            tr[y].latest = decision;
            tr[x].child[decision] = y;
        }
        // _cprintf("next Node %d\n", y);
        x = y;
    }
    *finalState = x;
    return 1;
}

std::pair<int, int> MCTree::UCTSearch() {
    for (int cnt=0;; cnt++) {
        int delta, finalState;
        bool succ = treePolicy(&delta, &finalState);
        //_cprintf("tree policy end\n");
        if (!succ) {
            _cprintf("terminate as all nodes are used %d\n", cnt);
            break;
        }
        backTrack(finalState, delta);
        //_cprintf("backTrack end\n");
        if (cnt % 1000 == 0 && timeout()) {
            _cprintf("terminate as timeout %d\n", cnt);
            break;
        }
    }
    Point ret;
    int decision = 0, mx = -1;
    for (int i=0; i<n; i++) {
        if (tr[root].child[i] && tr[tr[root].child[i]].cnt > mx) {
            mx = tr[tr[root].child[i]].cnt;
            decision = i;
        }
    }
    ret.x = decision;
    ret.y = board.getTop(decision);
    last = ret;
    _cprintf("my decision %d %d\n", ret.x, ret.y);
    for (int i = 0; i < n; i++) {
        if (tr[root].child[i]) {
            int y = tr[root].child[i];
            _cprintf("%d(%d): [x] %d %d [y] %d %d %f\n", y, i, tr[root].cnt, tr[root].value, tr[y].cnt, tr[y].value, tr[y].value*1.0/tr[y].cnt);
        }
        else {
            _cprintf("no (%d): %d\n", i, tr[root].child[i]);
        }
    }
    _cprintf("nodecnt %d\n", nodeCnt);
    //outputTree(root);
    return std::make_pair((int)ret.x, (int)ret.y);
}

void MCTree::outputTree(int x) {
    _cprintf("%d: fa %d latest %d cnt %d value %d\n", x, tr[x].fa, tr[x].latest, tr[x].cnt, tr[x].value);
    for (int i = 0; i < n; i++)
        _cprintf("%d ", tr[x].child[i]);
    _cprintf("\n");
    for (int i = 0; i < n; i++)
        if (tr[x].child[i])
            outputTree(tr[x].child[i]);
}
    


}