#ifndef __BOARD__
#define __BOARD__

#include "util.h"
#include <cstdlib>
namespace zc {

class Board {

public:
    Board(int n, int m, int noX, int noY);
    enum State {win, draw, running};
    inline bool canPut(int y) {
        return top[y] < m;
    }
    void put(int x);
    inline State getState() {
        return curState;
    }
    inline float prefer(int y) {
        return (n - abs(y - (n >> 1))) * 0.01;
    }
    inline bool isEnd() {
        return curState != running;
    }
    int evaluate();
    inline int getTop(int x) {
        return top[x];
    }
	void output();

private:
    bool verticalWin(int x, int y);
    bool horizentalWin(int x, int y);
    bool slantWin(int x, int y);
    bool a[12][12];
    int n, m, noX, noY;
    int top[12];
    State curState;
    int cnt, tot;
};

}
#endif // __BOARD__