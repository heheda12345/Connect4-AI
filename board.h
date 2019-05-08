#ifndef __BOARD__
#define __BOARD__

#include "util.h"
#include <cstdlib>
namespace zc {

class Board {

public:
    Board(int n, int m, int noX, int noY);
    enum State {win, draw, running};
    inline bool canPut(int y) const {
        return top[y] < m;
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
        return cnt & 1;
    }
    //bool haveUrgentPoint(int* col);
private:
    bool verticalWin(int x, int y);
    bool horizentalWin(int x, int y);
    bool slantWin(int x, int y);
    bool a[12][12];
    int n, m, noX, noY;
    int top[12];
    State curState;
    int cnt, tot;
    int liveUrgent[2][20]; //list by swap,  head liveUrgent[*][0]
    bool urgent[2][12][12];
};

}
#endif // __BOARD__