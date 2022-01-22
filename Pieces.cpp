/**
 * @file Pieces.cpp
 * @author Samuele Gallo
 */

#include "Pieces.h"
#include "chessBoard.h"

Pieces::Pieces(std::pair<int, int> pos, char col, char n, int moved)
{
    position = pos;
    Color = col;
    Name = n;
    Moved = moved;
}

void Pieces::SetMove(std::pair<int, int> NewPosition)
{
    position = NewPosition;
    Moved++;
}

std::pair<int, int> Pieces::GetPosition()
{
    return position;
}

char Pieces::GetColor()
{
    return Color;
}

int Pieces::GetStatus()
{
    return Moved;
}

//se il pezzo è nero ritorna la lettra corrispondente al pezzo maiuscola
//se il pezzo è bianco ritorna la lettera corrispondente al pezzo minuscola
char Pieces::GetName()
{
    if (Color == 'N')
        return Name;
    else
        return (Name + 32);
}

//costruttore e metodi dell'Alfiere
//aggiunge gli spostamenti nelle diagonali fino al limite della scacchiera
std::vector<std::vector<std::pair<int, int>>> A::Pmove()
{
    std::vector<std::vector<std::pair<int, int>>> Moves;
    int i = 1;

    std::vector<std::pair<int, int>> m1;
    while (position.first + i < 8 && position.second + i < 8)
    {
        m1.push_back(std::pair<int, int>(position.first + i, position.second + i));
        i++;
    }
    Moves.push_back(m1);
    i = 1;

    std::vector<std::pair<int, int>> m2;
    while (position.first + i < 8 && position.second - i >= 0)
    {
        m2.push_back(std::pair<int, int>(position.first + i, position.second - i));
        i++;
    }
    Moves.push_back(m2);
    i = 1;

    std::vector<std::pair<int, int>> m3;
    while (position.first - i >= 0 && position.second + i < 8)
    {
        m3.push_back(std::pair<int, int>(position.first - i, position.second + i));
        i++;
    }
    Moves.push_back(m3);
    i = 1;

    std::vector<std::pair<int, int>> m4;
    while (position.first - i >= 0 && position.second - i >= 0)
    {
        m4.push_back(std::pair<int, int>(position.first - i, position.second - i));
        i++;
    }
    Moves.push_back(m4);
    i = 1;

    return Moves;
}

A::A(std::pair<int, int> position, char color, int moved) : Pieces(position, color, 65, moved)
{}

//costruttore e metodi del Cavallo
//aggiunge gli spostamenti a "L" in tutte le direzioni
std::vector<std::vector<std::pair<int, int>>> C::Pmove()
{
    std::vector<std::vector<std::pair<int, int>>> Moves;
    std::vector<std::pair<int, int>> tmp;
    tmp.push_back(std::pair<int, int>(position.first + 2, position.second + 1));
    tmp.push_back(std::pair<int, int>(position.first + 2, position.second - 1));
    tmp.push_back(std::pair<int, int>(position.first - 2, position.second + 1));
    tmp.push_back(std::pair<int, int>(position.first - 2, position.second - 1));
    tmp.push_back(std::pair<int, int>(position.first + 1, position.second + 2));
    tmp.push_back(std::pair<int, int>(position.first - 1, position.second + 2));
    tmp.push_back(std::pair<int, int>(position.first + 1, position.second - 2));
    tmp.push_back(std::pair<int, int>(position.first - 1, position.second - 2));

    Moves.push_back(tmp);
    return Moves;
}

C::C(std::pair<int, int> position, char color, int moved) : Pieces(position, color, 67, moved)
{}

//costruttore e metodi della Donna
//aggiunge aggiunge gli spostamenti in tutte le direzioni fino al limite della scacchiera
std::vector<std::vector<std::pair<int, int> >> D::Pmove()
{
    std::vector<std::vector<std::pair<int, int>>> Moves;
    int i = 1;

    std::vector<std::pair<int, int>> m1;
    while (position.first + i < 8 && position.second + i < 8)
    {
        m1.push_back(std::pair<int, int>(position.first + i, position.second + i));
        i++;
    }
    Moves.push_back(m1);
    i = 1;

    std::vector<std::pair<int, int>> m2;
    while (position.first + i < 8 && position.second - i >= 0)
    {
        m2.push_back(std::pair<int, int>(position.first + i, position.second - i));
        i++;
    }
    Moves.push_back(m2);
    i = 1;

    std::vector<std::pair<int, int>> m3;
    while (position.first - i >= 0 && position.second + i >= 0)
    {
        m3.push_back(std::pair<int, int>(position.first - i, position.second + i));
        i++;
    }
    Moves.push_back(m3);
    i = 1;

    std::vector<std::pair<int, int>> m4;
    while (position.first - i >= 0 && position.second - i >= 0)
    {
        m4.push_back(std::pair<int, int>(position.first - i, position.second - i));
        i++;
    }
    Moves.push_back(m4);
    i = 1;

    //fine diagonali, inizio righe e colonne

    std::vector<std::pair<int, int>> m5;
    while (position.first + i < 8)
    {
        m5.push_back(std::pair<int, int>(position.first + i, position.second));
        i++;
    }
    Moves.push_back(m5);
    i = 1;

    std::vector<std::pair<int, int>> m6;
    while (position.first - i >= 0)
    {
        m6.push_back(std::pair<int, int>(position.first - i, position.second));
        i++;
    }
    Moves.push_back(m6);
    i = 1;

    std::vector<std::pair<int, int>> m7;
    while (position.second + i < 8)
    {
        m7.push_back(std::pair<int, int>(position.first, position.second + i));
        i++;
    }
    Moves.push_back(m7);
    i = 1;

    std::vector<std::pair<int, int>> m8;
    while (position.second - i >= 0)
    {
        m8.push_back(std::pair<int, int>(position.first, position.second - i));
        i++;
    }
    Moves.push_back(m8);
    i = 1;

    return Moves;
}

D::D(std::pair<int, int> position, char color, int moved) : Pieces(position, color, 68, moved)
{}

//costruttore e metodi del Pedone
//aggiunge gli spostamenti nella colonna in avanti di 1 o di 2 (se è la prima mossa)
std::vector<std::vector<std::pair<int, int> >> P::Pmove()
{
    std::vector<std::vector<std::pair<int, int>>> Moves;
    std::vector<std::pair<int, int>> tmp;

    int Col = 1;

    if (Color == 'N')
        Col = -1;

    tmp.push_back(std::pair<int, int>(position.first + Col, position.second));

    if (Moved == 0)
        tmp.push_back(std::pair<int, int>(position.first + (2 * Col), position.second));

    Moves.push_back(tmp);

    return Moves;
}

P::P(std::pair<int, int> position, char color, int moved) : Pieces(position, color, 80, moved)
{}

//costruttore e metodi del Re
//aggiunge gli spostamenti in tutte le direzioni di 1
std::vector<std::vector<std::pair<int, int> >> R::Pmove()
{
    std::vector<std::vector<std::pair<int, int>>> Moves;
    std::vector<std::pair<int, int>> tmp;

    tmp.push_back(std::pair<int, int>(position.first + 1, position.second));
    tmp.push_back(std::pair<int, int>(position.first - 1, position.second));
    tmp.push_back(std::pair<int, int>(position.first, position.second + 1));
    tmp.push_back(std::pair<int, int>(position.first, position.second - 1));

    //fine movimenti nelel righe e colonne, inizio nelle diagonali

    tmp.push_back(std::pair<int, int>(position.first + 1, position.second + 1));
    tmp.push_back(std::pair<int, int>(position.first - 1, position.second - 1));
    tmp.push_back(std::pair<int, int>(position.first + 1, position.second - 1));
    tmp.push_back(std::pair<int, int>(position.first - 1, position.second + 1));

    Moves.push_back(tmp);

    return Moves;
}

R::R(std::pair<int, int> position, char color, int moved) : Pieces(position, color, 82, moved)
{}

//costruttore e metodi della Torre
//aggiunge gli spostamenti nelle colonne e righe fino al limite della scacchiera
std::vector<std::vector<std::pair<int, int> >> T::Pmove()
{
    std::vector<std::vector<std::pair<int, int>>> Moves;
    int i = 1;

    std::vector<std::pair<int, int>> m1;
    while (position.first + i < 8)
    {
        m1.push_back(std::pair<int, int>(position.first + i, position.second));
        i++;
    }
    Moves.push_back(m1);
    i = 1;

    std::vector<std::pair<int, int>> m2;
    while (position.first - i >= 0)
    {
        m2.push_back(std::pair<int, int>(position.first - i, position.second));
        i++;
    }
    Moves.push_back(m2);
    i = 1;

    std::vector<std::pair<int, int>> m3;
    while (position.second + i < 8)
    {
        m3.push_back(std::pair<int, int>(position.first, position.second + i));
        i++;
    }
    Moves.push_back(m3);
    i = 1;

    std::vector<std::pair<int, int>> m4;
    while (position.second - i >= 0)
    {
        m4.push_back(std::pair<int, int>(position.first, position.second - i));
        i++;
    }
    Moves.push_back(m4);
    i = 1;

    return Moves;
}

T::T(std::pair<int, int> position, char color, int moved) : Pieces(position, color, 84, moved)
{}