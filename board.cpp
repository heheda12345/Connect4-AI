#include "board.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <conio.h>
using std::min;
using std::max;

namespace zc {
	Board::Board(int n_, int m_, int noX_, int noY_) :
		n(n_), m(m_), noX(noX_), noY(noY_), curState(running), cnt(0), tot(n*m-1), curPlayer(0) {
		// _cprintf("n: %d m: %d\n", n, m);
		memset(a, -1, sizeof(a));
        memset(urgent, 0, sizeof(urgent));
		for (int i = 0; i < n; i++) {
			top[i] = 0;
		}
        if (noY == 0)
            top[noX]++;
	}

    void Board::updatePoint(int sum, int ex, int ey) {
        switch (sum)
        {
        case 3:
            //_cprintf("find urgent %d %d\n", ex, ey);
            if (ex == noX && ey == noY)
                break;
            if (urgent[curPlayer][ex][ey])
                break;
            urgent[curPlayer][ex][ey] = 1;
            if (urgentList[curPlayer ^ 1].size())
                break;
            if (top[ex] <= ey-1 && urgent[curPlayer][ex][ey - 1]) {
                bool enermyUrgent = 0;
                for (int i = top[ex]; i <= ey; i++)
                    if (urgent[curPlayer ^ 1][ex][i])
                        enermyUrgent = 1;
                if (!enermyUrgent)
                    curState = win;
                //_cprintf("win reason 1 %d %d\n", ex, ey);
            }
            if (top[ex] <= ey && urgent[curPlayer][ex][ey + 1]) {
                bool enermyUrgent = 0;
                for (int i = top[ex]; i <= ey + 1; i++)
                    if (urgent[curPlayer ^ 1][ex][i])
                        enermyUrgent = 1;
                if (!enermyUrgent)
                    curState = win;
                //_cprintf("win reason 1 %d %d\n", ex, ey);
            }
            if (top[ex] == ey) {
                urgentList[curPlayer].insert(ex);
                if (urgentList[curPlayer].size() >= 2) {
                    curState = win;
                    //_cprintf("win reason 2 %d %d\n", ex, ey);
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

    void Board::checkHorizental(int x, int y) {
        // _cprintf("check horizental\n");
        int left = max(x - 3, 0), right = left + 3;
        int sum = 0, lastEmpty = -1;
        int rival = curPlayer ^ 1;

        for (int i = left; i <= right; i++)
            if (a[i][y] == curPlayer)
                sum += 1;
            else if (a[i][y] == rival)
                sum -= 1;
            else lastEmpty = i;
        updatePoint(sum, lastEmpty, y);

        for (left++, right++; right < n && left <= x; left++, right++) {
            if (a[left-1][y] == curPlayer)
                sum -= 1;
            else if (a[left-1][y] == rival)
                sum += 1;
            
            if (a[right][y] == curPlayer)
                sum += 1;
            else if (a[right][y] == rival)
                sum -= 1;
            else
                lastEmpty = right;
            updatePoint(sum, lastEmpty, y);
        }
    }

    void Board::checkVertical(int x, int y) {
        // _cprintf("check vertical\n");
        int down = max(y - 3, 0), up = down + 3;
        int sum = 0, lastEmpty = -1;
        int rival = curPlayer ^ 1;

        for (int j = down; j <= up; j++)
            if (a[x][j] == curPlayer)
                sum += 1;
            else if (a[x][j] == rival)
                sum -= 1;
            else lastEmpty = j;
        updatePoint(sum, x, lastEmpty);
        for (down++, up++; up < m && down <= y; up++, down++) {
            if (a[x][down - 1] == curPlayer)
                sum -= 1;
            else if (a[x][down - 1] == rival)
                sum += 1;

            if (a[x][up] == curPlayer)
                sum += 1;
            else if (a[x][up] == rival)
                sum -= 1;
            else
                lastEmpty = up;
            updatePoint(sum, x, lastEmpty);
        }
    }

    void Board::checkSlant1(int x, int y) {
        // _cprintf("check slant 1\n");
        int sum = 0, lastEmptyX = -1, lastEmptyY = -1;
        int rival = curPlayer ^ 1;
        int der = min(min(x, y), 3);
        int left = x - der, down = y - der, right = left + 3, up = down + 3;
        if (right >= n || up >= m)
            return;

        for (int i = left, j = down; i <= right; i++, j++)
            if (a[i][j] == curPlayer)
                sum++;
            else if (a[i][j] == rival)
                sum--;
            else
                lastEmptyX = i, lastEmptyY = j;
        updatePoint(sum, lastEmptyX, lastEmptyY);

        for (left++, right++, up++, down++; left <= x && right < n && up < m; left++, right++, up++, down++) {
            if (a[left - 1][down - 1] == curPlayer)
                sum--;
            else if (a[left - 1][down - 1] == rival)
                sum++;

            if (a[right][up] == curPlayer)
                sum++;
            else if (a[right][up] == curPlayer)
                sum--;
            else
                lastEmptyX = right, lastEmptyY = up;
            updatePoint(sum, lastEmptyX, lastEmptyY);
        }
    }

    void Board::checkSlant2(int x, int y) {
        // _cprintf("check slant 2\n");
        int sum = 0, lastEmptyX = -1, lastEmptyY = -1;
        int rival = curPlayer ^ 1;
        int der = min(min(x, m - y - 1), 3);
        int left = x - der, right = left + 3, up = y + der, down = up - 3;
        if (right >= n || down < 0)
            return;

        for (int i = left, j = up; i <= right; i++, j--)
            if (a[i][j] == curPlayer)
                sum++;
            else if (a[i][j] == rival)
                sum--;
            else
                lastEmptyX = i, lastEmptyY = j;
        updatePoint(sum, lastEmptyX, lastEmptyY);

        for (left++, right++, up--, down--; left <= x && right < n && down >= 0; left++, right++, up--, down--) {
            if (a[left - 1][up + 1] == curPlayer)
                sum--;
            else if (a[left - 1][up + 1] == rival)
                sum++;

            if (a[right][down] == curPlayer)
                sum++;
            else if (a[right][down] == curPlayer)
                sum--;
            else
                lastEmptyX = right, lastEmptyY = down;
            updatePoint(sum, lastEmptyX, lastEmptyY);
        }
    }

	void Board::put(int x) {
		// without validity checking
		// assume state = running
		//_cprintf("put %d\n", x);
		int y = top[x];
		a[x][y] = curPlayer;
        cnt++;
        top[x]++;
        if (x == noX && top[x] == noY)
            top[x]++;
        if (curState == win)
            curState = lose;
        if (cnt == tot && curState == running)
            curState = draw;
        
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

        // output();
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