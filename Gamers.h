/**
 * @file Gamers.h
 * @author Samuele Gallo
 */
#ifndef GAMERS_H
#define GAMERS_H

#include "chessBoard.h"

class Gamers
{

protected:
    char Color;                                     //colore dei pezzi del giocatore
    ChessBoard *chessBoard; //scacchiera

public:
    //costruttore
    Gamers(char C, ChessBoard *chessBoard);         //costruttore dei due tipi di giocatori (bot e giocatore umano)

    //player
    bool Move(std::string start, std::string end);  //metodo che esegue la mossa per il player
    int GetCondition();                             //metodo che restituisce se la condizione del giocatore (vedere chessboard.h per la lista delle possibili condizioni)
    void PerformPromotion(char code);               //metodo che fa effettuare la promozione di un pedone per il player

    //bot
    bool Move();                                    //metodo che esegue la mossa per il bot
    char PerformPromotion();                        //metodo che fa effettuare la promozione di un pedone per il bot
};

#endif
