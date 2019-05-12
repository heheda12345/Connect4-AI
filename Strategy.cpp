#include <iostream>
#include "Point.h"
#include "Strategy.h"
#include "util.h"
#include "MCTree.h"
#include <conio.h>
#include <atlstr.h>
using namespace std;

/*
    ���Ժ����ӿ�,�ú������Կ�ƽ̨����,ÿ�δ��뵱ǰ״̬,Ҫ�����������ӵ�,�����ӵ������һ��������Ϸ��������ӵ�,��Ȼ�Կ�ƽ̨��ֱ����Ϊ��ĳ�������
    
    input:
        Ϊ�˷�ֹ�ԶԿ�ƽ̨ά����������ɸ��ģ����д���Ĳ�����Ϊconst����
        M, N : ���̴�С M - ���� N - ���� ����0��ʼ�ƣ� ���Ͻ�Ϊ����ԭ�㣬����x��ǣ�����y���
        top : ��ǰ����ÿһ���ж���ʵ��λ��. e.g. ��i��Ϊ��,��_top[i] == M, ��i������,��_top[i] == 0
        _board : ���̵�һά�����ʾ, Ϊ�˷���ʹ�ã��ڸú����տ�ʼ���������Ѿ�����ת��Ϊ�˶�ά����board
                ��ֻ��ֱ��ʹ��board���ɣ����Ͻ�Ϊ����ԭ�㣬�����[0][0]��ʼ��(����[1][1])
                board[x][y]��ʾ��x�С���y�еĵ�(��0��ʼ��)
                board[x][y] == 0/1/2 �ֱ��Ӧ(x,y)�� ������/���û�����/�г������,�������ӵ㴦��ֵҲΪ0
        lastX, lastY : �Է���һ�����ӵ�λ��, ����ܲ���Ҫ�ò�����Ҳ������Ҫ�Ĳ������ǶԷ�һ����
                ����λ�ã���ʱ��������Լ��ĳ����м�¼�Է������ಽ������λ�ã�����ȫȡ�������Լ��Ĳ���
        noX, noY : �����ϵĲ������ӵ�(ע:��ʵ���������top�Ѿ����㴦���˲������ӵ㣬Ҳ����˵���ĳһ��
                ������ӵ�����ǡ�ǲ������ӵ㣬��ôUI�����еĴ�����Ѿ������е�topֵ�ֽ�����һ�μ�һ������
                ��������Ĵ�����Ҳ���Ը�����ʹ��noX��noY��������������ȫ��Ϊtop������ǵ�ǰÿ�еĶ�������,
                ��Ȼ�������ʹ��lastX,lastY�������п��ܾ�Ҫͬʱ����noX��noY��)
        ���ϲ���ʵ���ϰ����˵�ǰ״̬(M N _top _board)�Լ���ʷ��Ϣ(lastX lastY),��Ҫ���ľ�������Щ��Ϣ�¸������������ǵ����ӵ�
    output:
        ������ӵ�Point
*/

zc::MCTree* mcTree = nullptr;
extern "C" __declspec(dllexport) Point* getPoint(const int M, const int N, const int* top, const int* _board, 
    const int lastX_, const int lastY_, const int noX_, const int noY_){
    /*
        ��Ҫ������δ���
    */
    int x = -1, y = -1;//���ս�������ӵ�浽x,y��
    int** board = new int*[M];
    for(int i = 0; i < M; i++){
        board[i] = new int[N];
        for(int j = 0; j < N; j++){
            board[i][j] = _board[i * N + j];
        }
    }
    
    /*
        �������Լ��Ĳ������������ӵ�,Ҳ���Ǹ�����Ĳ�����ɶ�x,y�ĸ�ֵ
        �ò��ֶԲ���ʹ��û�����ƣ�Ϊ�˷���ʵ�֣�����Զ����Լ��µ��ࡢ.h�ļ���.cpp�ļ�
    */
    //Add your own code below
    // AllocConsole();
    int n=M, m=N;
    zc::MCTree::timerStart();
    int lastX = lastY_, lastY = M-lastX_-1;
    int noX = noY_, noY = M-noX_-1;

    if (zc::isNewGame(N, M, board)) {
        // _cprintf("newGame!\n");
        if (mcTree != nullptr)
            delete mcTree;
        bool playerID = (lastX != -1) && (lastY != -1);
        mcTree = new zc::MCTree(playerID, N, M, noX, noY);
        if (playerID)
            mcTree->changeRootTo(make_pair(lastX, lastY));
    } else {
        // _cprintf("oldGame!\n");
        mcTree->changeRootTo(mcTree->getLastDecision());
        // _cprintf("changeRootTo end 1\n");
        mcTree->changeRootTo(make_pair(lastX, lastY));
        // _cprintf("changeRootTo end 2\n");
    }
    // _cprintf("start UCT\n");
    std::pair<int, int> p = mcTree->UCTSearch();
    // _cprintf("end UCT\n");
    y = p.first;
    x = M - p.second - 1;
    
    /*
        ��Ҫ������δ���
    */
    clearArray(M, N, board);
    return new Point(x, y);
}


/*
    getPoint�������ص�Pointָ�����ڱ�dllģ���������ģ�Ϊ��������Ѵ���Ӧ���ⲿ���ñ�dll�е�
    �������ͷſռ䣬����Ӧ�����ⲿֱ��delete
*/
extern "C" __declspec(dllexport) void clearPoint(Point* p){
    delete p;
    return;
}

/*
    ���top��board����
*/
void clearArray(int M, int N, int** board){
    for(int i = 0; i < M; i++){
        delete[] board[i];
    }
    delete[] board;
}


/*
    ������Լ��ĸ�������������������Լ����ࡢ����������µ�.h .cpp�ļ�������ʵ������뷨
*/
