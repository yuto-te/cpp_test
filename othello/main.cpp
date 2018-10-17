/**
 * @file main.cpp
 * @brief Othello
 * @author yuto-te
 */

#include <iostream>
#include <array>
#include <fstream>      // ofstream
#include <sstream>      // stringstream
#include <unistd.h>     // sleep
#include <vector>

constexpr int L = 10; // オセロの格子のサイズ，境界用に一回り大きくとっている

class Othello{
private:
    std::array< std::array<int, L>, L > board;
    int turn, not_turn; // 手番の色を管理
    bool pass1, pass2; // 連続するパスの管理
    std::vector< std::vector<int> > list_can_put; // 石を置ける座標のリスト
    std::string filename; // 出力ファイル名
    int check(const int x, const int y, const int p, const int q);
    bool check_around(const int x, const int y);
    void turn_over(const int x, const int y, const int p, const int q, const int reverse);
public:
    Othello();
    void update(const int x, const int y);
    void print();
    void write_file();
    void search();
    bool pass_check();
    std::array<int, 2> next_stone();
    bool end_of_game();
};

Othello::Othello()
    : turn(1) // 黒が先攻
    , not_turn(2) // 白が後攻
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

    // 現在時刻をファイル名としてログをとる
    time_t t = time(nullptr);
    const tm* now_time = localtime(&t);
    std::stringstream s;
    s<<"20";
    s<<now_time->tm_year-100 <<now_time->tm_mon+1 <<now_time->tm_mday << now_time->tm_hour << now_time->tm_min << now_time->tm_sec << ".dat";
    filename = s.str();
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

/**
 * @brief ひっくり返る医師の個数を調べて，ひっくり返す
 */
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
 * @brief 周囲8方向でひっくり返すことのできる石の個数の合計を求めて，0でないか調べる
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

/**
 * @brief 石のおける場所のリストを返す
 */
void Othello::search(){
    std::vector< std::vector<int> > can_put;
    for(int x = 1; x < L - 1; x++){
        for(int y = 1; y < L - 1; y++){
            if(board[x][y] == 0){
                if(check_around(x, y)) can_put.push_back({x, y});
            }
        }
    }
    list_can_put = can_put; // 石を置ける座標のリスト
} // 出力ファイル名
/**
 * @brief 打てる場所がなければパスする，パスが連続で続くかも判定する
 */
bool Othello::pass_check(){
    if(list_can_put.size() == 0){ // 石を置ける座標のリスト
        std::swap(turn, not_turn); // 出力ファイル名
        if(pass1) pass2 = true;
        else pass1 = true;
        return true;
    }
    else return false;
}

/**
 * @brief パスが連続したら(連続でお互いにパスしたら)ゲーム終了
 */
bool Othello::end_of_game(){
    if(pass1 && pass2) return true;
    else return false;
}

/**
 * @brief 入力した座標に石を置けるか判定する
 */
bool if_put(const int x, const int y, const std::vector< std::vector<int> > &can_put){
    bool flag = false;
    for(auto&& coordinate : can_put){
        if(x == coordinate[0] && y == coordinate[1]){
            flag = true;
            break;
        }
    }
    return flag;
}

/**
 * @brief 盤面を出力する
 */
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
            else if(if_put(x, y, list_can_put)){ // 石を置ける座標のリスト
                std::cout << "　"; // 出力ファイル名
            }
            else std::cout << "・";
        }
        std::cout << std::endl;
    }
    std::cout << "黒 " << black << "   " << "白 " << white << std::endl;
}

/**
 * @brief 途中経過をファイルに保存する
 */
void Othello::write_file(){
    std::ofstream writing_file;
    writing_file.open(filename, std::ios::app);
    writing_file << "  A B C D E F G H" << std::endl;
    int black = 0;
    int white = 0;
    for(int x = 1; x < L - 1; x++){
        writing_file << x;
        for(int y = 1; y < L - 1; y++){
            if(board[x][y] == 1){
                writing_file << " o";
                black++;
            }
            else if(board[x][y] == 2){
                writing_file << " #";
                white++;
            }
            else if(if_put(x, y, list_can_put)){ // 石を置ける座標のリスト
                writing_file << "  "; // 出力ファイル名
            }
            else writing_file << " .";
        }
        writing_file << std::endl;
    }
    writing_file << "黒 " << black << "   " << "白 " << white << std::endl;
    writing_file << std::endl;
    writing_file.close();
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

/**
 * @brief 次に石を置く場所を入力する
 */
std::array<int, 2> Othello::next_stone(){
    char c;
    int x, y;
    pass1 = pass2 = false;
    while(true){
        if(turn == 1) std::cout << "黒";
        else std::cout << "白";
        std::cin >> x >> c;
        y = alphabetToNumber(c);
        if(if_put(x, y, list_can_put)) break; // 石を置ける座標のリスト
        std::cout << "wrong place, again" << std::endl; // 出力ファイル名
    }
    return {x, y};
}

int main(){
    Othello game;
    std::array<int, 2> coordinate;
    while(true){
        if(game.end_of_game()) break;
        std::system("clear");
        game.search();
        if(game.pass_check()){
            // 手番交代
            game.search();
            game.print();
            game.write_file();
            std::cout << "pass" << std::endl;
            sleep(1);
            continue;
        }
        else{
            game.print();
            game.write_file();
            coordinate = game.next_stone();
            game.update(coordinate[0], coordinate[1]);
        }
    }
    std::cout << "end game" << std::endl;
    return 0;
}
