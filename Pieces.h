#ifndef PIECES_H
#define PIECES_H

#include <vector>

class Pieces
{

        protected:

        std::pair<int, int> position;
        char Color;
        int Moved;
        char Name;

    public:
        Pieces(std::pair<int, int> position, char color, char n, int moved);

        void SetMove(std::pair<int, int> NewPosition);
        std::pair<int, int> GetPosition();

        char GetColor(); 
        char GetName();

        int GetStatus();


        virtual std::vector<std::vector<std::pair<int, int>>> Pmove() = 0;
};

class A : public Pieces
{
    public:
        A(std::pair<int, int> position, char color, int moved = 0);
        std::vector<std::vector<std::pair<int, int> >> Pmove() override;
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