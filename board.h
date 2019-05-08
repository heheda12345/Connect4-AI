#ifndef __BOARD__
#define __BOARD__

#include "util.h"

namespace zc {

class Board {

public:
    Board(int n, int m, int noX, int noY);
    enum State {win, draw, running};
    bool canPut(int y);
    void put(int x);
    State getState();
    float prefer(int y);
    bool isEnd();
    int evaluate();
    int getTop(int x) {
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