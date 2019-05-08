#include "board.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <conio.h>


namespace zc {
	Board::Board(int n_, int m_, int noX_, int noY_) :
		n(n_), m(m_), noX(noX_), noY(noY_), curState(running), cnt(0), tot(n*m-1) {
		_cprintf("n: %d m: %d\n", n, m);
		memset(a, -1, sizeof(a));
		for (int i = 0; i < n; i++) {
			top[i] = 0;
		}
        if (noY == 0)
            top[noX]++;
	}

	bool Board::canPut(int y) {
		return top[y] < m;
	}

    bool Board::horizentalWin(int x, int y) {
        int left = x, right = x;
        bool curPlayer = a[x][y];
        while (left > 0 && a[left - 1][y] == curPlayer)
            left--;
        while (right < n - 1 && a[right + 1][y] == curPlayer)
            right++;
        return right - left + 1 >= 4;
    }

    bool Board::verticalWin(int x, int y) {
        int up = y, down = y;
        bool curPlayer = a[x][y];
        while (up < m - 1 && a[x][up + 1] == curPlayer)
            up++;
        while (down > 0 && a[x][down - 1] == curPlayer)
            down--;
        return up - down + 1 >= 4;
    }

    bool Board::slantWin(int x, int y) {
        int x1 = x, y1 = y, x2 = x, y2 = y;
        bool curPlayer = a[x][y];
        while (x1 > 0 && y1 > 0 && a[x1 - 1][y1 - 1] == curPlayer)
            x1--, y1--;
        while (x2 < n - 1 && y2 < m - 1 && a[x2 + 1][y2 + 1] == curPlayer)
            x2++, y2++;
        if (x2 - x1 + 1 >= 4)
            return 1;

        x1 = x, y1 = y, x2 = x, y2 = y;
        while (x1 > 0 && y1 < m - 1 && a[x1 - 1][y1 + 1] == curPlayer)
            x1--, y1++;
        while (x2 < n - 1 && y2 > 0 && a[x2 + 1][y2 - 1] == curPlayer)
            x2++, y2--;
        if (x2 - x1 + 1 >= 4)
            return 1;
        
        return 0;
    }

	void Board::put(int x) {
		// without validity checking
		// assume state = running
		//_cprintf("put %d\n", x);
		int y = top[x];
		bool curPlayer = cnt & 1;
		a[x][y] = cnt & 1;
		
        if (verticalWin(x, y) || horizentalWin(x, y) || slantWin(x, y))
            curState = win;
        else if (cnt == tot - 1)
            curState = draw;
		cnt++;
		top[x]++;
		if (x == noX && top[x] == noY)
			top[x]++;
	}

	Board::State Board::getState() {
		return curState;
	}

	float Board::prefer(int y) {
		return (n - abs(y - (n >> 1)))*0.01;
	}

	bool Board::isEnd() {
		return curState != running;
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
		default:
			printf("error in Board::evaluate\n");
			break;
		}
	}

	void Board::output() {
		_cprintf("=============================\n");
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
		_cprintf("=============================\n");
	}
}