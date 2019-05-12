#ifndef __CONSTANTS__
#define __CONSTANTS__

namespace zc {
// const int MAX_TREE_NODE = 10000000;
const int MAX_TREE_NODE = 10000000;
const int MAX_WIDTH = 12;
const int MC_TIME_LIMIT = 2700; // TODO should be 2700
const int INF = 1000000000;
const float UCT_C = float(0.8);
const float UCT_UNREACHED_CONFIDENT = 100; // TODO

const unsigned char win_buf[2] = {170, 255}; // 10101010 11111111
const unsigned char threat_buf[2][4] = {
    {42, 138, 162, 168}, // 00101010 10001010 10100010 10101000
    {63, 207, 243, 252}
};

}

#endif //__CONSTANTS__