#ifndef GAMERS_H
#define GAMERS_H

#include "chessBoard.h"

class Gamers
{

    protected:

        string Name;
        char Color;
        ChessBoard *chessBoard;
        char Type;

    public:

//costruttore
        Gamers(char C, ChessBoard *chessBoard, string N, char T);

//player
        bool Move(string start, string end);
        int GetCondition();
        void PerformPromotion(char code);
        char PerformPromotion();

        //bot
        bool Move();
};

#endif

