#include "MCTree.h"
#include <cstring>
#include <cmath>
#include <cstdio>
#include "constants.h"
#include <conio.h>
#include <assert.h>
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
    //_cprintf("value %d\n", value);
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


int MCTree::treePolicy(int x, Board* board) {
    while (!board->isEnd()) {
        int expandNodes[12], expandNode = 0;
        int existNodes[12], existNode = 0;
        int decision = -1;
        if (!board->haveUrgentPoint(&decision)) {
            for (int i = 0; i < n; i++) {
                if (board->canPut(i)) {
                    if (tr[x].child[i]) {
                        existNodes[existNode++] = i;
                    }
                    else {
                        expandNodes[expandNode++] = i;
                    }
                }
            }
            if (expandNode) {
                decision = expandNodes[rand() % expandNode];
            }
            else {
                float mx = -INF;
                for (int i = 0; i < existNode; i++) {
                    int y = tr[x].child[existNodes[i]];
                    float confident = tr[y].value * 1.0 / tr[y].cnt + UCT_C * sqrt(2 * log(tr[x].cnt * 1.0 / tr[y].cnt));
                    if (confident > mx) {
                        decision = existNodes[i];
                        mx = confident;
                    }
                }
                assert(decision != -1); //TODO 有可能快输了导致没法放子
            }
        }
        if (!tr[x].child[decision]) {
            int y = newNode(x);
            assert(y > 0);
            tr[y].latest = decision;
            tr[x].child[decision] = y;
            board->put(decision);
            return y;
        }
        else {
            board->put(decision);
            x = tr[x].child[decision];
        }
    }
    return x;
}

int MCTree::defaultPolicy(int x, Board* board) {
    int curPlayer = board->nextPlayer();
    while (!board->isEnd()) {
        int decision;
        if (!board->haveUrgentPoint(&decision)) {
            while (1) {
                decision = rand() % n;
                if (board->canPut(decision))
                    break;
            }
        }
        board->put(decision);
        // _cprintf("is urgent: %d\n", board->haveUrgentPoint(&decision));
    }
    int delta = board->evaluate();
    return curPlayer == board->nextPlayer() ? delta : -delta;
}

std::pair<int, int> MCTree::UCTSearch() {
    int decision = -1;
    if (this->board.haveUrgentPoint(&decision)) {
        goto End;
    }
    int UCTStart = root;
    for (int cnt=0;; cnt++) {
        Board board = this->board;
        int x = treePolicy(UCTStart, &board);
        if (!x) {
            _cprintf("terminate as all nodes are used %d\n", cnt);
            break;
        }
        int delta = defaultPolicy(x, &board);
        backTrack(x, delta);
        if (cnt % 1000 == 0 && timeout()) {
            _cprintf("terminate as timeout %d\n", cnt);
            break;
        }
    }
    if (decision == -1) {
        int mx = -1;
        for (int i = 0; i < n; i++) {
            if (tr[root].child[i] && tr[tr[root].child[i]].cnt > mx) {
                mx = tr[tr[root].child[i]].cnt;
                decision = i;
            }
        }
    }
End:
    Point ret;
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
    this->board.output();
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