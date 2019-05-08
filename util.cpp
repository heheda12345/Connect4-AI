#include "util.h"
#include <conio.h>
#include <atlstr.h>
namespace zc {

bool isNewGame(int n, int m, int **board) {
    /* warning: board is in TA's format */
    int cnt = 0;
    for (int i=0; i<m; i++)
        for (int j=0; j<n; j++)
            if (board[i][j] != 0)
                cnt++;
    return cnt < 2;
}

}