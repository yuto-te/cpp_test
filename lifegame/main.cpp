/**
 * @file main.cpp
 * @brief Game of Life
 * @author yuto-te
 */

#include <iostream>
#include <ctime>        // time
#include <cstdlib>      // srand,rand
#include <string>
#include <unistd.h>     // sleep
#include <vector>

/**
 * @brief class of life game field
 */
class LifeGame{
private:
    const int _size;
    std::vector< std::vector<bool> > field;
    bool at_cell(const int &x, const int &y);
    bool dead_or_alive(const int &x, const int &y);
public:
    LifeGame(const int L);
    void update();
    void print(const int t);
};

/**
 * @brief initialize field
 * @param[in] L size of lattice
 */
LifeGame::LifeGame(const int L)
    : _size(L)
{
    const int threshold = 10;
    field.resize(_size);
    std::srand(time(NULL)); // seed of random number
    for(int x = 0; x < _size; x++){
        field[x].resize(_size);
        for(int y = 0; y < _size; y++){
            if(rand() % (2*threshold) < threshold) field[x][y] = true;
            else field[x][y] = false;
        }
    }
}

/**
 * @brief judge if the cell is dead or alive
 * @param[in] x field coordinate, y field coordinate
 * @param[out] bool cell condition
 * @details 範囲外参照を防ぐ．範囲外はすべてdead．
 */
bool LifeGame::at_cell(const int &x, const int &y){
    if(x < 0 || x >= _size || y < 0 || y >= _size) return false;
    else return field[x][y];
}

/**
 * @brief update cell to next time
 * @param[in] x field coordinate, y field coordinate
 * @param[out] bool if alive
 * @details 1つのセルについて，aliveかつ周囲8セルのうち2つまたは3つがaliveのとき次もalive．deadかつ周囲8セルのうち3つがaliveのとき次はalive．その他は次はdead．
 */
bool LifeGame::dead_or_alive(const int &x, const int &y){
    auto cell = at_cell(x, y);
    auto count = at_cell(x - 1, y - 1) + at_cell(x    , y - 1) + at_cell(x + 1, y - 1)
               + at_cell(x - 1, y    )                         + at_cell(x + 1, y    )
               + at_cell(x - 1, y + 1) + at_cell(x    , y + 1) + at_cell(x + 1, y + 1);
    if(count == 3) return true;
    else if (cell && count == 2) return true;
    else return false;
}

/**
 * @brief update field
 */
void LifeGame::update(){
    for(int x = 0; x < _size; x++){
        for(int y = 0; y < _size; y++){
            field[x][y] = dead_or_alive(x, y);
        }
    }
}

/**
 * @brief print life game on command prompt
 * @param[in] t time
 */
void LifeGame::print(const int t){
    std::system("clear");
    std::cout << t << "[s]" << std::endl;
    for(auto&& row : field){
        for(auto&& cell : row){
            std::cout << (cell ? "■" : "□");
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

/**
 * @brief main function
 */
int main()
{
    int L; // size of lattice
    int TIME_MAX; // time of end
    std::cout << "lattice size?" << std::endl;
    std::cin >> L;
    std::cout << "end time?" << std::endl;
    std::cin >> TIME_MAX;

    LifeGame game(L);
    int t = 0;
    while(t <= TIME_MAX){
        game.print(t);
        game.update();
        t++;
        usleep(1000000);
    }
    return 0;
}
