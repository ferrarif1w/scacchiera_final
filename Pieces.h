/**
 * @file Pieces.h
 * @author Samuele Gallo
 */
#ifndef PIECES_H
#define PIECES_H

#include <vector>

class Pieces
{

        protected:

        std::pair<int, int> position;                                           //coppia di coordinate del pezzo
        char Color;                                                             //colore del pezzo
        int Moved;                                                              //numero di mosse esseguite dal pezzo durante una partita
        char Name;                                                              //nome del pezzo

    public:
        Pieces(std::pair<int, int> position, char color, char n, int moved);    //costruttore del pezzo (coordinate, colore, nome, stato)

        void SetMove(std::pair<int, int> NewPosition);                          //metodo che aggiorna la posizione e incrementa il numero di mosse
        std::pair<int, int> GetPosition();                                      //metodo che restituisce la posizione attuale del pezzo

        char GetColor();                                                        //metodo che restituisce la posizione attuale del pezzo
        char GetName();                                                         //metodo che restituisce il nome del pezzo
        int GetStatus();                                                        //metodo che restituisce lo stato del pezzo (moved)

        virtual std::vector<std::vector<std::pair<int, int>>> Pmove() = 0;      //vettore che contiene tutte le possibili mosse teoriche del pezzo (senza controllare presenza di altri pezzi, scacchi, etc..)
                                                                                //se il pezzo può eseguire più mosse nella stessa direzione allora avrò più vettori (uno per direzione) e viene controllato che tutte le posizioni siano all'interno della scacchiera
};                                                                              //se vi è un solo vettore allora sarà una lista di mosse senza controllo che siano dentro la scacchiera

class A : public Pieces
{
    public:
        A(std::pair<int, int> position, char color, int moved = 0);
        std::vector<std::vector<std::pair<int, int>>> Pmove() override;
};

class C : public Pieces
{
    public:
        C(std::pair<int, int> position, char color, int moved = 0);
        std::vector<std::vector<std::pair<int, int> >> Pmove() override;
};

class D : public Pieces
{
    public:
        D(std::pair<int, int> position, char color, int moved = 0);
        std::vector<std::vector<std::pair<int, int> >> Pmove() override;
};

class P : public Pieces
{
    public:
        P(std::pair<int, int> position, char color, int moved = 0);
        std::vector<std::vector<std::pair<int, int> >> Pmove() override;
};

class R : public Pieces
{
    public:
        R(std::pair<int, int> position, char color, int moved = 0);
        std::vector<std::vector<std::pair<int, int> >> Pmove() override;
};

class T : public Pieces
{
    public:
        T(std::pair<int, int> position, char color, int moved = 0);
        std::vector<std::vector<std::pair<int, int> >> Pmove() override;
};

#endif