/**
 * @file main.cpp
 * @brief Othello
 * @author yuto-te
 */

#include <iostream>
#include <array>
#include <unistd.h>     // sleep
#include <vector>

constexpr int L = 10; // オセロの格子のサイズ

class Othello{
private:
    std::array< std::array<int, L>, L > board;
    int turn, not_turn;
    bool pass1, pass2;
    std::vector< std::vector<int> > you_can_put;
    int check(const int x, const int y, const int p, const int q);
    bool check_around(const int x, const int y);
    void turn_over(const int x, const int y, const int p, const int q, const int reverse);
public:
    Othello();
    void update(const int x, const int y);
    void print();
    void search();
    bool pass_check();
    std::array<int, 2> next_stone();
    bool end_of_game();
};

Othello::Othello()
    : turn(1)
    , not_turn(2)
    , pass1(false)
    , pass2(false)
{
    for(int x = 0; x < L; x++){
        for(int y = 0; y < L; y++){
            board[x][y] = 0;
        }
    }
    board[4][4] = board[5][5] = 1;
    board[5][4] = board[4][5] = 2;
    // board[2][6] = board[2][7] = board[2][8] = board[3][5] = board[3][7] = board[5][3] = board[5][4] = board[5][5] = 1;
    // board[1][6] = board[1][7] = board[1][8] = board[3][6] = board[3][8] = board[4][3] = board[4][4] = board[4][5] = board[4][6] = board[5][6] = 2;
}

/**
 * @brief 石を置いたときにある方向についてひっくり返る石の個数を返す
 * @param[in] x, y: 置いた石の座標, p, q: チェックする方向
 * @param[out] reverse: ひっくり返る個数
 */
int Othello::check(const int x, const int y, const int p, const int q){
    int i = x + p, j = y + q;
    int reverse = 0;
    while(i >= 0 && i < L && j >= 0 && j < L){
        if(board[i][j] == 0){
            reverse = 0;
            break;
        }
        else if(board[i][j] == turn) break;
        else reverse++;
        i += p;
        j += q;
    }
    return reverse;
}

/**
 * @brief 石を置いたときにある方向について指定した個数石をひっくり返す
 * @param[in] x, y: 置いた石の座標, p, q: チェックする方向, reverse: ひっくり返す個数
 */
void Othello::turn_over(const int x, const int y, const int p, const int q, const int reverse){
    int i = x, j = y;
    for(int n = 0; n <= reverse; n++){
        board[i][j] = turn;
        i += p;
        j += q;
    }
}

void Othello::update(const int x, const int y){
    std::array<int, 3> a = {-1, 0, 1};
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(i == 1 && j == 1) continue;
            int reverse = check(x, y, a[i], a[j]);
            turn_over(x, y, a[i], a[j], reverse);
        }
    }
    std::swap(turn, not_turn);
}

/**
 * @brief 周囲8方向でひっくり返すことのできる石の個数を返す
 */
bool Othello::check_around(const int x, const int y){
    std::array<int, 3> a = {-1, 0, 1};
    int reverse = 0;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(i == 1 && j == 1) continue;
            reverse += check(x, y, a[i], a[j]);
        }
    }
    if(reverse == 0) return false;
    else return true;
}

void Othello::search(){
    std::vector< std::vector<int> > can_put;
    for(int x = 0; x < L; x++){
        for(int y = 0; y < L; y++){
            if(board[x][y] == 0){
                if(check_around(x, y)) can_put.push_back({x, y});
            }
        }
    }
    you_can_put = can_put;
}

bool Othello::pass_check(){
    if(you_can_put.size() == 0){
        std::swap(turn, not_turn);
        if(pass1) pass2 = true;
        else pass1 = true;
        if(pass1 && pass2) return false;
        else return true;
    }
    else return false;
}

bool Othello::end_of_game(){
    if(pass1 && pass2) return true;
    else return false;
}

bool if_put(const int x, const int y, const std::vector< std::vector<int> > &can_put){
    bool flag = false;
    for(auto&& pair : can_put){
        if(x == pair[0] && y == pair[1]){
            flag = true;
            break;
        }
    }
    return flag;
}

void Othello::print(){
    std::cout << " ＡＢＣＤＥＦＧＨ" << std::endl;
    int black = 0;
    int white = 0;
    for(int x = 1; x < L - 1; x++){
        std::cout << x;
        for(int y = 1; y < L - 1; y++){
            if(board[x][y] == 1){
                std::cout << "○";
                black++;
            }
            else if(board[x][y] == 2){
                std::cout << "●";
                white++;
            }
            else if(if_put(x, y, you_can_put)){
                std::cout << "　";
            }
            else std::cout << "・";
        }
        std::cout << std::endl;
    }
    std::cout << "黒 " << black << "   " << "白 " << white << std::endl;
}

/**
 * @brief アルファベットを数値に変換する. A-H -> 0-7
 * @param[in] c alphabet character
 */
int alphabetToNumber(const char c){
    if('A' <= c && c <= 'H') return (c - 'A' + 1);
    else if('a' <= c && c <= 'h') return (c - 'a' + 1);
    return -1;
}

std::array<int, 2> Othello::next_stone(){
    char c;
    int x, y;
    pass1 = pass2 = false;
    while(true){
        if(turn == 1) std::cout << "黒";
        else std::cout << "白";
        std::cin >> x >> c;
        y = alphabetToNumber(c);
        if(if_put(x, y, you_can_put)) break;
        std::cout << "wrong place, again" << std::endl;
    }
    return {x, y};
}

int main(){
    Othello game;
    std::array<int, 2> pair;
    while(true){
        std::system("clear");
        game.search();
        if(game.pass_check()){
            game.search();
            game.print();
            std::cout << "pass" << std::endl;
            sleep(1);
            continue;
        }
        else game.print();
        if(game.end_of_game()) break;
        pair = game.next_stone();
        game.update(pair[0], pair[1]);
    }
    std::cout << "end game" << std::endl;
    return 0;
}
