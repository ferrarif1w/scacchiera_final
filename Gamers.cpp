#include "Gamers.h"

Gamers::Gamers(char C, ChessBoard *B, std::string N, char T)
{
    Name = N;
    Color = C;
    chessBoard = B;
    Type = T;
}

//player
bool Gamers::Move(std::string start, std::string end)
{
    int sFirst = start[1]-49;
    int sSecond = start[0]-65;
    if (sSecond > 7) sSecond -= 32;
    int fFirst = end[1]-49;
    int fSecond = end[0]-65;
    if (fSecond > 7) fSecond -= 32;
    std::pair<int, int> s = std::pair<int, int>(sFirst, sSecond);
    std::pair<int, int> f = std::pair<int, int>(fFirst, fSecond);

return chessBoard->performMove(s, f);
}

int Gamers::GetCondition()
{
    return chessBoard->getCondition(Color);

}

void Gamers::PerformPromotion(char code)
{
    char c = code;
    if (c > 90) c -= 32;
    chessBoard->performPromotion(c);
}

//Bot
bool Gamers::Move()
{
    return chessBoard->performMove();
}

char Gamers::PerformPromotion()
{

    char P[4]{'A', 'T', 'C', 'D'};

    int r = rand() % 4;

    chessBoard->performPromotion(P[r]);

    return P[r];
}
