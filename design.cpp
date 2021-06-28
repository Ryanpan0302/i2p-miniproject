#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include<bits/stdc++.h>
using namespace std;
#define c 30//corner
#define e 10//edge
#define d -10//half middle
#define b 30//
#define a 5//middle
#define m 0//middle

#define f 15
#define g 6
#define h 3
#define k 4
#define l 3

const int valuetable1[8][8]={
     99, -8,  8,  6,  6,  8, -8, 99,
     -8,-24, -4, -3, -3, -4,-24, -8,
      8, -4,  7,  4,  4,  7, -4,  8,
      6, -3,  4,  0,  0,  4, -3,  6,
      6, -3,  4,  0,  0,  4, -3,  6,
      8, -4,  7,  4,  4,  7, -4,  8,
     -8,-24, -4, -3, -3, -4,-24, -8,
     99, -8,  8,  6,  6,  8, -8, 99 };

const int valuetable3[8][8]={
    c,m,e,e,e,e,m,c,
    m,d,d,d,d,d,d,m,
    e,d,b,b,b,b,d,e,
    e,d,b,a,a,b,d,e,
    e,d,b,a,a,b,d,e,
    e,d,b,b,b,b,d,e,
    m,d,d,d,d,d,d,m,
    c,m,e,e,e,e,m,c };

const int valuetable2[8][8]={
    f,g,g,g,g,g,g,f,
    g,h,h,h,h,h,h,g,
    g,h,k,k,k,k,h,g,
    g,h,k,l,l,k,h,g,
    g,h,k,l,l,k,h,g,
    g,h,k,k,k,k,h,g,
    g,h,h,h,h,h,h,g,
    f,g,g,g,g,g,g,f };

enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
};

struct Point {
    int x, y;
    Point() : Point(0, 0) {}
	Point(int x, int y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};

const std::array<Point, 8> directions{{
    Point(-1, -1), Point(-1, 0), Point(-1, 1),
    Point(0, -1), /*{0, 0}, */Point(0, 1),
    Point(1, -1), Point(1, 0), Point(1, 1)
}};

int player;
int white=0,black=0,blank=0;//
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;

class OthelloBoard {
public:
    std::array<std::array<int, SIZE>, SIZE> oboard;
    std::vector<Point> nxt_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;
private:
    int get_next_player(int player) const {
        return 3 - player;//1<=>2
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return oboard[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        oboard[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(this->cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, this->cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(this->cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, this->cur_player)) {//到下一個同色旗
                    for (Point s: discs) {
                        set_disc(s, this->cur_player);
                    }
                    disc_count[this->cur_player] += discs.size();
                    disc_count[get_next_player(this->cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
public:
    OthelloBoard(){}
    OthelloBoard(std::array<std::array<int, SIZE>, SIZE> newboard){//copy constructor
        disc_count[EMPTY] = 0;
        disc_count[BLACK] = 0;
        disc_count[WHITE] = 0;
        this->cur_player=player;
        this->done=false;
        this->winner=-1;
        this->nxt_valid_spots=next_valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                this->oboard[i][j] = newboard[i][j];
                if(oboard[i][j]==EMPTY)disc_count[EMPTY]++;
                else if(oboard[i][j]==BLACK)disc_count[BLACK]++;
                else if(oboard[i][j]==WHITE)disc_count[WHITE]++;
            }
        }
    }
    OthelloBoard(const OthelloBoard& origin){//copy constructor
        this->cur_player=origin.cur_player;
        this->disc_count=origin.disc_count;
        this->done=origin.done;
        this->nxt_valid_spots=origin.nxt_valid_spots;
        this->oboard=origin.oboard;
        this->winner=origin.winner;
    }
    /*OthelloBoard& operator = (const OthelloBoard &origin){//copy constructor
        this->cur_player=origin.cur_player;
        this->disc_count=origin.disc_count;
        this->done=origin.done;
        this->next_valid_spots=origin.next_valid_spots;
        this->oboard=origin.oboard;
        this->winner=origin.winner;
        return *this;
    }*/
    void reset() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                oboard[i][j] = EMPTY;
            }
        }
        oboard[3][4] = oboard[4][3] = BLACK;
        oboard[3][3] = oboard[4][4] = WHITE;
        this->cur_player = BLACK;
        disc_count[EMPTY] = 8*8-4;
        disc_count[BLACK] = 2;
        disc_count[WHITE] = 2;
        nxt_valid_spots = get_valid_spots();
        done = false;
        winner = -1;
    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (oboard[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        /*if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }*/
        set_disc(p, this->cur_player);
        disc_count[this->cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        this->cur_player = get_next_player(this->cur_player);
        this->nxt_valid_spots = get_valid_spots();
        // Check Win
        /*if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs) winner = BLACK;
                else winner = WHITE;
            }
        }*/
        return true;
    }
    /*void nextboard(){//要改
        int count;
        int n_valid_spots = next_valid_spots.size();
        for(int i=0;i<n_valid_spots;i++){
            Point center = next_valid_spots[i];
            for (Point dir: directions) {
                // Move along the direction while testing.
                Point p = center + dir;
                Point op=p;
                if(board[p.x][p.y]!=BLACK)
                    continue;
                p = p + dir;
                count=0;
                while(board[p.x][p.y]!=EMPTY && onboard(p)==true){
                    if(board[p.x][p.y]==WHITE){
                        for(int j=0;j<count+1;j++){
                            board[op.x+j*dir.x][op.y+j*dir.y]=WHITE;
                        }
                        board[center.x][center.y]=WHITE;
                        break;
                    }
                    p = p + dir;
                    count++;
                }
            }
        }
    }*/
    int valuefunc(){/**/
        int heur=0;
        if(player==1){
            for(int i=0;i<SIZE;i++){
                for(int j=0;j<SIZE;j++){
                    if(disc_count[EMPTY]>=52){
                        if(oboard[i][j]==BLACK)heur+=valuetable1[i][j];
                        else if(oboard[i][j]==WHITE)heur-=valuetable1[i][j];
                    }else if(disc_count[EMPTY]>=8){
                        if(oboard[i][j]==BLACK)heur+=valuetable1[i][j];
                        else if(oboard[i][j]==WHITE)heur-=valuetable1[i][j];
                    }else{
                        if(oboard[i][j]==BLACK)heur+=5;
                        else if(oboard[i][j]==WHITE)heur-=5;
                    }
                }
            }
            //heur+=(disc_count[BLACK]-disc_count[WHITE])*7;
            //return disc_count[BLACK]-disc_count[WHITE];
        }
        else{
            for(int i=0;i<SIZE;i++){
                for(int j=0;j<SIZE;j++){
                    if(disc_count[EMPTY]>=52){
                        if(oboard[i][j]==WHITE)heur+=valuetable1[i][j];
                        else if(oboard[i][j]==BLACK)heur-=valuetable1[i][j];
                    }else if(disc_count[EMPTY]>=8){
                        if(oboard[i][j]==WHITE)heur+=valuetable1[i][j];
                        else if(oboard[i][j]==BLACK)heur-=valuetable1[i][j];
                    }else{
                        if(oboard[i][j]==WHITE)heur+=5;
                        else if(oboard[i][j]==BLACK)heur-=5;
                    }
                }
            }
            //heur+=(disc_count[WHITE]-disc_count[BLACK])*7;
            //return disc_count[WHITE]-disc_count[BLACK];
        } 
        return heur;
    }
};

void read_board(std::ifstream& fin) {//讀近來直接建一個新的board
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}



/*function alphabeta(node, depth, α, β, maximizingPlayer) is
    if depth = 0 or node is a terminal node then
        return the heuristic value of node
    if maximizingPlayer then
        value := −∞
        for each child of node do
            value := max(value, alphabeta(child, depth − 1, α, β, FALSE))
            if value ≥ β then
                break (* β cutoff *)
            α := max(α, value)
        return value
    else
        value := +∞
        for each child of node do
            value := min(value, alphabeta(child, depth − 1, α, β, TRUE))
            if value ≤ α then
                break (* α cutoff *)
            β := min(β, value)
        return value*/
int alphabeta(OthelloBoard otboard,int depth,int alpha,int beta,bool maxplayer){
    //otboard.cur_player=(maxplayer?1:2);//確認遺下
    if(depth==0 || (otboard.nxt_valid_spots).size()==0){
        return otboard.valuefunc();
    }
    if(maxplayer){
        int value=-INT_MAX;
        vector<Point> children=otboard.nxt_valid_spots;
        int n_children=children.size();
        for(int i=0;i<n_children;i++){
            OthelloBoard child(otboard);//make new board
            child.put_disc(children[i]);//add disc
            //child.cur_player=3-board.cur_player;//child的player不一樣 //put_disc就換了
            value=max(value, alphabeta(child, depth-1, alpha, beta, false));
            alpha=max(value,alpha);
            if(alpha>=beta) break;
        }
        return value;
    }else{
        int value=INT_MAX;
        vector<Point> children=otboard.nxt_valid_spots;
        int n_children=children.size();
        for(int i=0;i<n_children;i++){
            OthelloBoard child(otboard);//make new board
            child.put_disc(children[i]);//add disc
            //child.cur_player=3-board.cur_player;//child的player不一樣
            value=min(value, alphabeta(child, depth-1, alpha, beta, true));
            beta=min(value,beta);
            if(beta<=alpha) break;
        }
        return value;
    }
}
void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

void write_valid_spot(std::ofstream& fout) {
    int n_valid_spots = next_valid_spots.size();
    OthelloBoard otboard(board);
    //srand(time(NULL));
    // Choose random spot. (Not random uniform here)
    //int index = (rand() % n_valid_spots);
    int heuristic=-INT_MAX,index;
    for(int i=0;i<n_valid_spots;i++){
        OthelloBoard child(otboard);//make new board
        child.put_disc(next_valid_spots[i]);//add disc
        fout << next_valid_spots[i].x << " " << next_valid_spots[i].y << std::endl;
        if(alphabeta(child,10, -INT_MAX, INT_MAX, true)>heuristic){//let depth=10
            index=i;
        }   
    }
    Point p = next_valid_spots[index];
    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
