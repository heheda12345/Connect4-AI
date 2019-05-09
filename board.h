#ifndef __BOARD__
#define __BOARD__

#include "util.h"
#include <cstdlib>
#include <assert.h>
#include <algorithm>
namespace zc {

class Board {

public:
    Board(int n, int m, int noX, int noY);
    enum State {win, draw, running};
    inline bool canPut(int x) const {
        if (top[x] >= m)
            return 0;
        if (!urgent[curPlayer][x][top[x]] && urgent[curPlayer ^ 1][x][top[x] + 1])
            return 0;
        return 1;
    }
    void put(int x);
    inline State getState() const {
        return curState;
    }
    inline float prefer(int y) const {
        return (n - abs(y - (n >> 1))) * 0.01;
    }
    inline bool isEnd() const {
        return curState != running;
    }
    int evaluate();
    inline int getTop(int x) const {
        return top[x];
    }
	void output();
    inline bool nextPlayer() const {
        return curPlayer;
    }
    bool haveUrgentPoint(int* col) {
        if (!urgentList[curPlayer].empty()) {
            *col = urgentList[curPlayer].item();
            return 1;
        }
        if (!urgentList[curPlayer ^ 1].empty()) {
            *col = urgentList[curPlayer ^ 1].item();
            return 1;
        }
        return 0;
    }
private:
    void checkVertical(int x, int y);
    void checkHorizental(int x, int y);
    void checkSlant1(int x, int y);
    void checkSlant2(int x, int y);
    void updatePoint(int sum, int ex, int ey);
    struct UrgentList {
        int q[20];
        int w;

        UrgentList() : w(0) {}

        inline void insert(int x) {
            q[++w] = x;
        }
        void remove(int x) {
            for (int i=1; i<=w; i++)
                if (q[i] == x) {
                    std::swap(q[w], q[i]);
                    w--;
                    return;
                }
            assert(false);
        }

        inline bool empty() {
            return w == 0;
        }
        inline int item() {
            return q[w];
        }
        inline int size() {
            return w;
        }
    };
    bool a[12][12];
    int n, m, noX, noY;
    int top[12];
    State curState;
    int cnt, tot;
    int curPlayer;
    UrgentList urgentList[2];
    bool urgent[2][13][13];
};

}
#endif // __BOARD__