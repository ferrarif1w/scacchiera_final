/**
 * @file Gamers.cpp
 * @author Samuele Gallo
 */
#include "Gamers.h"

Gamers::Gamers(char C, ChessBoard *B)
{
    Color = C;
    chessBoard = B;
}

bool Gamers::Move(std::string start, std::string end)
{
    //si trsforma la lettera della riga/colonna in un valore corrispondente (0-7) per la scacchiera
    int sFirst = start[1]-49;               //1
    int sSecond = start[0] - 65;            //A
    //l'if serve nel caso il carattere sia una lettera minuscola
    if (sSecond > 7) sSecond -= 32;        //32 è la distanza nella tabella ascii tra lettere maiuscole e minuscole

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
    if (c > 90) c -= 32;                //se la lettera è minuscola diventa minuscola
    chessBoard->performPromotion(c);
}

bool Gamers::Move()
{
    return chessBoard->performMove();
}

char Gamers::PerformPromotion()
{
    char P[4]{'A', 'T', 'C', 'D'};      //array dei nomi dei pezzi in cui il pedone può promuoversi

    int r = rand() % 4;                 //estrazione casuoale con le stesse probabilità tra le opzioni per la promozione dei pedoni

    chessBoard->performPromotion(P[r]);

    return P[r];
}
