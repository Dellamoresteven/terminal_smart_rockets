// g++ --std=c++17 -pthread -o run main.cpp rocket.cpp && time ./run
/*
real	0m3.593s
user	0m3.580s
sys	0m0.016s
*/
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <thread>

#include "rocket.cpp"

using namespace std;
// PARAMS TO CHANGE
#define NUM_THREADS 5
constexpr int moves = 200;
constexpr int numRockets = 200;
const std::pair<int,int> targetLoc = std::pair(4,4);
constexpr double mutationRate = .1;
constexpr double numGames = 2000;
// END

#define s 50

void drawMap();
void initMap();
void scoreRockets(vector<rocket*> r);
void newGeneration(vector<rocket*> &r);

int map[s][s];
int minScore = 1000;


void *wait(void *t) {

}

int main() {
    // pthread_t threads[NUM_THREADS];
    // pthread_attr_t attr;
    // void *status;

    // // Initialize and set thread joinable
    // pthread_attr_init(&attr);
    // pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    bool gameOver = false;
    initMap();
    srand (time(NULL));
    auto randDir = []() {
        vector<std::string> ret;
        for(int i = 0; i < moves; i++) {
            int r = rand() % 3;
            if(r == 0) {
                ret.push_back("U");
            } else if(r == 1) {
                ret.push_back("R");
            } else if(r == 2) {
                ret.push_back("L");
            }
        }
        return ret;
    };
    vector<rocket*> r;
    for(int i = 0; i < numRockets; i++) {
        r.push_back( new rocket(randDir(), map) );
    }

    auto playgame = [&](bool show) {
        // pthread_create(&threads[i], &attr, wait, (void *)i );
        while(!gameOver) {
            if(show) usleep(1000 * 1000 * .01);

            // for(int k = 0; k < NUM_THREADS; k++) {
            //     pthread_create(&threads[k], &attr, wait, (void *)k );
            // }

            for(int i = 0; i < r.size(); i++) {
                rocket * r1 = r.at(i);
                if(!r1->gotToTarget) {
                    r1->move(targetLoc);
                    map[r1->y][r1->x] = 2;
                    if(!(r1->y == r1->ly) || !(r1->x == r1->lx)){
                        map[r1->ly][r1->lx] = 0;
                    }
                }
                if(r1->index == moves){
                    gameOver = true;
                    break;
                }
            }


            if(show) drawMap();
        }
    };

    auto gameloop = [&](bool show){
        gameOver = false;
        initMap();
        playgame(show);
        scoreRockets(r);
        newGeneration(r);
    };

    for(int i = 0; i < numGames; i++) {
        gameloop(false);
    }
    gameloop(true);
    // for(const auto rocket : r) {
    //     cout << rocket->gotToTarget << endl;
    // }
}

void scoreRockets(vector<rocket*> r) {
    int rocketI = 0;
    double avg = 0;
    minScore = 100;
    for(auto rocket : r) {
        int dist = sqrt(pow(rocket->x - targetLoc.second, 2) + pow(rocket->y - targetLoc.first, 2));
        rocket->score = dist;
        if(minScore > dist) minScore = dist;
        avg += dist;
    }
    cout << avg/r.size() << endl;
    // cout << "min: " << minScore << endl;
}

void newGeneration(vector<rocket*> &r) {
    sort(r.begin(), r.end(), [](auto r1, auto r2) {
        r1->index = 0;
        r2->index = 0;
        // if(r2->gotToTarget) return true;
        return r1->score < r2->score;
    });

    delete r[r.size()-1];
    vector<std::string> newDNA;
    for(int i = 0; i < moves/2; i++) {
        double rm2 = rand() % 100/100.0;
        if(rm2 < mutationRate) {
            int r = rand() % 3;
            if(r == 0) {
                newDNA.push_back("U");
            } else if(r == 1) {
                newDNA.push_back("R");
            } else if(r == 2) {
                newDNA.push_back("L");
            }
        } else {
            int whichParent = rand() % 2;
            if(whichParent == 0) newDNA.push_back(r.at(0)->directions.at(i));
            else if(whichParent == 1) newDNA.push_back( r.at(1)->directions.at(i));
        }
    }
    r.at(r.size() - 1) = new rocket(newDNA, map);
}

void initMap() {

    for(int i = 0; i < s; i++) {
        for(int j = 0; j < s; j++) {
            map[i][j] = 0;
        }
    }

    for(int i = 0; i < s; i++) {
        for(int j = 0; j < s; j++) {
            if(i == 0 || j == 0 || j == s-1) {
                map[i][j] = 1;
            }
        }
    }

    for(int i = 0; i < 25; i++) {
        if(!(i == 20)) {
            map[20][i] = 1;
            map[20][s-i] = 1;
        }
    }

    for(int i = 0; i < s; i++) {
        if(i != 25) {
            map[15][i] = 1;
        }
    }

    // for(int i = 0; i < s; i++) {
    //     if(i != 45) {
    //         map[10][i] = 1;
    //     }
    // }


    for(int i = 0; i < s; i++) {
        if(i != 3 && i != 35) {
            map[35][i] = 1;
        }
    }

    for(int i = 0; i < s; i++) {
        if(!(i>20 && i < 28)) {
            map[27][i] = 1;
        }
    }
    map[targetLoc.first][targetLoc.second] = 3;
}

void drawMap() {
    cout << "\n\n";
    for(int i = 0; i < s; i++) {
        for(int j = 0; j < s; j++) {
            // cout << map[i][j] << " ";
            switch(map[i][j]) {
                case 0:
                    cout << "  ";
                    break;
                case 1:
                    cout << "- ";
                    break;
                case 2:
                    cout << "I ";
                    break;
                case 3:
                    cout << "X ";
                    break;
            }
        }
        cout << "\n";
    }
}