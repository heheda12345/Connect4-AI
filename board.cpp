#include "board.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <conio.h>
#include "constants.h"
using std::min;
using std::max;

namespace zc {
	Board::Board(int n_, int m_, int noX_, int noY_) :
		n(n_), m(m_), noX(noX_), noY(noY_), curState(running), cnt(0), tot(n*m-1), curPlayer(0) {
		//_cprintf("n: %d m: %d noX %d noY %d\n", n, m, noX, noY);
		memset(a, -1, sizeof(a));
        memset(urgent, 0, sizeof(urgent));
		for (int i = 0; i < n; i++) {
			top[i] = 0;
		}
        if (noY == 0)
            top[noX]++;
        unsigned heng_initial = (1 << ((n + 1) << 1)) | 1;
        for (int i = 0; i < m; i++)
            heng[i] = heng_initial;
        heng[noY] |= 1 << ((noX+1) << 1);

        unsigned shu_initial = (1 << ((m + 1) << 1)) | 1;
        for (int i = 0; i < n; i++)
            shu[i] = shu_initial;
        shu[noX] |= 1 << ((noY + 1) << 1);

        memset(xie1, 0, sizeof(xie1)); // (x, y) => (x-y+m+1, x+1)
        for (int x = -1; x <= n; x++) { // (x,-1) (x,m)
            xie1[x + 1 + m + 1] |= 1 << ((x + 1) << 1);
            xie1[x - m + m + 1] |= 1 << ((x + 1) << 1);
        }
        for (int y = -1; y <= m; y++) {// (-1, y) (n, y)
            xie1[-1 - y + m + 1] |= 1 << ((-1 + 1) << 1);
            xie1[n - y + m + 1] |= 1 << ((n + 1) << 1);
        }
        xie1[noX - noY + m + 1] |= 1 << ((noX + 1) << 1);
        
        memset(xie2, 0, sizeof(xie2)); // (x, y) => (x+y+2, x+1) 
        for (int x = -1; x <= n; x++) { // (x, -1) (x, m)
            xie2[x - 1 + 2] |= 1 << ((x + 1) << 1);
            xie2[x + m + 2] |= 1 << ((x + 1) << 1);
        }
        for (int y = -1; y <= m; y++) { // (-1, y) (n, y)
            xie2[-1 + y + 2] |= 1 << ((-1 + 1) << 1);
            xie2[n + y + 2] |= 1 << ((n + 1) << 1);
        }
        xie2[noX + noY + 2] |= 1 << ((noX + 1) << 1);

	}

    void Board::updatePoint(int sum, int ex, int ey) {
        switch (sum)
        {
        case 3:
            if (ex == noX && ey == noY)
                break;
            if (urgent[curPlayer][ex][ey])
                break;
            urgent[curPlayer][ex][ey] = 1;
            if (urgentList[curPlayer ^ 1].size())
                break;
            if (top[ex] == ey - 1 && urgent[curPlayer][ex][ey - 1]) {
                curState = win;
                //_cprintf("win reason 1 %d %d\n", ex, ey);
            }
            if (top[ex] == ey) {
                urgentList[curPlayer].insert(ex);
                if (urgentList[curPlayer].size() >= 2) {
                    curState = win;
                    //_cprintf("win reason 2 %d %d\n", ex, ey);
                }
                else if (urgent[curPlayer][ex][ey + 1]) {
                    curState = win;
                    //_cprintf("win reason 3 %d %d\n", ex, ey);
                }
            }
            break;
        case 4:
            //_cprintf("win as sum=4\n");
            curState = win;
            break;
        default:
            break;
        }
    }

    void Board::updateThreat(int x, int y) {
        //_cprintf("update threat %d %d\n", x, y);
        if (urgent[curPlayer][x][y])
            return;
        urgent[curPlayer][x][y] = 1;
        if (urgentList[curPlayer ^ 1].size())
            return;
        if (top[x] == y - 1 && urgent[curPlayer][x][y - 1]) {
            curState = win;
            //_cprintf("win reason 1 %d %d\n", ex, ey);
        }
        if (top[x] == y) {
            urgentList[curPlayer].insert(x);
            if (urgentList[curPlayer].size() >= 2) {
                curState = win;
                //_cprintf("win reason 2 %d %d\n", ex, ey);
            }
            else if (urgent[curPlayer][x][y + 1]) {
                curState = win;
                //_cprintf("win reason 3 %d %d\n", ex, ey);
            }
        }
    }

    void Board::checkHorizental(int x, int y) {
        unsigned cur = heng[y];
        //_cprintf("cur %u\n", cur);
        for (int bias = -2; bias <= 1; bias ++) {
            unsigned char get = (cur >> ((x + bias) << 1)) & 255;
            //_cprintf("get %u\n", unsigned(get));
            for (int i = 0; i < 4; i++) {
                if (get == threat_buf[curPlayer][i]) {
                    updateThreat(x + bias - i + 2, y);
                }
            }
        }
    }

    void Board::checkVertical(int x, int y) {
        unsigned cur = shu[x];
        unsigned char get = (cur >> ((y - 1) << 1)) & 255;
        //_cprintf("cur %u get %u\n", cur, unsigned(get));
        if (get == threat_buf[curPlayer][0])
            updateThreat(x, y + 1);
    }

    void Board::checkSlant1(int x, int y) {
        unsigned cur = xie1[x - y + m + 1];
        //_cprintf("cur %u\n", cur);
        for (int bias = -2; bias <= 1; bias++) {
            unsigned char get = (cur >> ((x + bias) << 1)) & 255;
            //_cprintf("get %u\n", unsigned(get));
            for (int i = 0; i < 4; i++) {
                if (get == threat_buf[curPlayer][i]) {
                    updateThreat(x + bias - i + 2, y + bias - i + 2);
                }
            }
        }
    }

    void Board::checkSlant2(int x, int y) {
        unsigned cur = xie2[x + y + 2];
        // _cprintf("cur %u\n", cur);
        for (int bias = -2; bias <= 1; bias++) {
            unsigned char get = (cur >> ((x + bias) << 1)) & 255;
            // _cprintf("get %u\n", unsigned(get));
            for (int i = 0; i < 4; i++) {
                if (get == threat_buf[curPlayer][i]) {
                    updateThreat(x + bias - i + 2, y - bias + i - 2);
                }
            }
        }
    }
    

	void Board::put(int x) {
		// without validity checking
		// assume state = running
        int y = top[x];
        // _cprintf("put %d %d\n", x, y);
        a[x][y] = curPlayer; // TO DELETE
        heng[y] |= (2 | curPlayer) << ((x + 1) << 1);
        shu[x] |= (2 | curPlayer) << ((y + 1) << 1);
        xie1[x - y + m + 1] |= (2 | curPlayer) << ((x + 1) << 1);
        xie2[x+y+2] |= (2 | curPlayer) << ((x + 1) << 1);
        cnt++;
        top[x]++;
        if (x == noX && top[x] == noY)
            top[x]++;
        if (curState == win)
            curState = lose;
        if (curState == running) {
            if (urgent[curPlayer][x][y])
                curState = win;
            else if (cnt == tot)
                curState = draw;
        }
        if (urgent[curPlayer][x][y])
            urgentList[curPlayer].remove(x);
        if (urgent[curPlayer ^ 1][x][y])
            urgentList[curPlayer ^ 1].remove(x);

        if (urgent[curPlayer][x][top[x]])
            urgentList[curPlayer].insert(x);
        if (urgent[curPlayer ^ 1][x][top[x]])
            urgentList[curPlayer ^ 1].insert(x);

        checkHorizental(x, y);
        checkVertical(x, y);
        checkSlant1(x, y);
        checkSlant2(x, y);
        curPlayer ^= 1;
        bool canPutExist = 0;
        for (int i=0; i<n; i++)
            if (canPut(i)) {
                canPutExist = 1;
                break;
            }
        if (!canPutExist && curState == running)
            curState = win;

        //output();
	}

	int Board::evaluate() {
		switch (curState)
		{
		case draw:
			return 0;
			break;
		case win:
			return 1;
			break;
        case lose:
            return -1;
            break;
		default:
			// printf("error in Board::evaluate\n");
            assert(false);
			break;
		}
	}

	void Board::output() {
		_cprintf("=============================\n");
        if (curState == win)
            _cprintf("win!\n");
        _cprintf("top: ");
        for (int i = 0; i < n; i++)
            _cprintf("%d ", top[i]);
        _cprintf("\n");
		for (int j = m - 1; j >= 0; j--) {
			for (int i = 0; i < n; i++)
				if (i == noX && j == noY)
					_cprintf("x");
				else if (a[i][j] == 0)
					_cprintf("0");
				else if (a[i][j] == 1)
					_cprintf("1");
				else
					_cprintf(".");
			_cprintf("\n");
		}

        for (int k = 0; k < 2; k++) {
            _cprintf("urgent%d: ", k);
            for (int i = 1; i <= urgentList[k].w; i++)
                _cprintf("%d ", urgentList[k].q[i]);
            _cprintf("\n");
            for (int j = m - 1; j >= 0; j--) {
                for (int i = 0; i < n; i++)
                    _cprintf("%d", urgent[k][i][j]);
                _cprintf("\n");
            }
        }

		_cprintf("=============================\n");
	}
}