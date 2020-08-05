#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>

using namespace std;

constexpr int s = 50;

class rocket {

    public:
    int x;
    int y;
    int lx;
    int ly;
    vector<std::string> directions;
    int index;
    int map[s][s];
    int score = 1000;
    int movesTakenToReachMin = 0;
    int minDist = 1000;
    bool gotToTarget = false;
    rocket(vector<std::string> d, int m[s][s]) {
        x = s/2;
        y = s-1;
        lx = x;
        ly = y;
        index = 0;
        directions = d;
        memcpy(map, m, s*s*sizeof(s));
    }

    void move(std::pair<int,int> target) {
        if(gotToTarget) return;
        lx = x;
        ly = y;
        if(index >= directions.size()) {
            return;
        }

        if(directions.at(index) == "U" && map[y-1][x] != 1) {
            y--;
        } else if(directions.at(index) == "R" && map[y][x+1] != 1) {
            x++;
        } else if(directions.at(index) == "L" && map[y][x-1] != 1) {
            x--;
        } else if(directions.at(index) == "D" && y+1 < s && map[y+1][x] != 1) {
            // y++;
        }
        int dist = (sqrt(pow(x - target.second, 2) + pow(y - target.first, 2)));

        if(dist == 0) {
            gotToTarget = true;
        }
        index++;
    }

    std::pair<int, int> getPos() {
        return std::pair<int,int>(x,y);
    }
};