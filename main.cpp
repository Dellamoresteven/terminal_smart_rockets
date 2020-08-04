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
constexpr int moves = 300;
constexpr int numRockets = 200;
const std::pair<int,int> targetLoc = std::pair(4,4);
constexpr double percentTake = .2;
constexpr double mutationRate = .03;
constexpr double numGames = 102;
// END

#define s 50

void drawMap();
void initMap();
void scoreRockets(vector<rocket*> r);
vector<rocket*> newGeneration(vector<rocket*> r);

int map[s][s];
int minScore = 1000;


void *wait(void *t) {

}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;
    void *status;

    // Initialize and set thread joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    bool gameOver = false;
    initMap();
    srand (time(NULL));
    auto randDir = []() {
        vector<std::string> ret;
        for(int i = 0; i < moves; i++) {
            int r = rand() % 4;
            if(r == 0) {
                ret.push_back("U");
            } else if(r == 1) {
                ret.push_back("R");
            } else if(r == 2) {
                ret.push_back("L");
            } else if(r == 3) {
                ret.push_back("N");
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
                if(!r1->isGodParent) {
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
        for(int i = 0; i < s; i++) {
            for(int j = 0; j < s; j++) {
                map[i][j] = 0;
            }
        }
        initMap();
        // std::thread first(playgame(show));
        playgame(show);
        scoreRockets(r);
        r = newGeneration(r);
    };

    for(int i = 0; i < numGames; i++) {
        gameloop(false);
    }

    gameloop(true);
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
    // cout << avg/r.size() << endl;
    cout << "min: " << minScore << endl;
}

vector<rocket*> newGeneration(vector<rocket*> r) {
    vector<rocket*> nr;
    auto randDir = []() {
        vector<std::string> ret;
        for(int i = 0; i < moves; i++) {
            int r = rand() % 4;
            if(r == 0) {
                ret.push_back("U");
            } else if(r == 1) {
                ret.push_back("R");
            } else if(r == 2) {
                ret.push_back("L");
            } else if(r == 3) {
                ret.push_back("N");
            }
        }
        return ret;
    };
    int numTake = r.size() * percentTake;
    sort(r.begin(), r.end(), [](auto r1, auto r2){
        if(r2->isGodParent) {
            return true;
        }
        return r1->score < r2->score;
    });

    // for(auto x : r) {
    //     cout << x->score << endl;
    // }

    for(int i = 0; i < r.size(); i++) {
        double rm2 = rand() % 100/100.0;
        int r1 = rand() % numTake;
        int r2 = rand() % numTake;
        rocket * p1 = r.at(rand() % numTake);
        rocket * p2 = r.at(rand() % numTake);
        vector<std::string> newDNA;
        for(int j = 0; j < moves; j++) {
            double rm = rand() % 100/100.0;
            if(rm < mutationRate) {
                int r = rand() % 4;
                if(r == 0) {
                    newDNA.push_back("U");
                } else if(r == 1) {
                    newDNA.push_back("R");
                } else if(r == 2) {
                    newDNA.push_back("L");
                } else if(r == 3) {
                    newDNA.push_back("N");
                }
            } else {
                int whichParent = rand() % 2;
                if(whichParent == 0) newDNA.push_back(p1->directions.at(j));
                else if(whichParent == 1) newDNA.push_back(p2->directions.at(j));
            }
        }
        // if(rm2 < mutationRate) { // Brand new Random DNA
        //     nr.push_back( new rocket(randDir(), map) );
        // } else {
            nr.push_back( new rocket(newDNA, map) );
        // }

    }

    // deleteteteette
    for ( int i = 0; i < r.size(); i++) {
        delete r[i];
    }
    return nr;
}

void initMap() {
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