#ifndef __UTIL__
#define __UTIL__
namespace zc {

// class Point {
// public:
//     Point() = default;
//     Point(int x, int y) : pp((x << 4) | y){}
//     Point(const Point&) = default;
//     inline unsigned char x() { return pp >> 4; }
//     inline unsigned char y() { return pp & 15; }
// private:
//     unsigned char pp;
// };

struct Point {char x,y;};

bool isNewGame(int n, int m, int **board);

}
#endif //__UTIL__
