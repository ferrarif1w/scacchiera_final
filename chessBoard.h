#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <algorithm>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <random>
#include "Pieces.h"

class ChessBoard {
    public:
        //pezzo, destinazione, codice della mossa, eventuale pezzo aggiuntivo
        struct Move {
            //pezzo mosso
            Pieces* piece;
            //posizione di arrivo
            std::pair<int, int> destination;
            //0: mossa normale, 1: mossa normale con cattura,
            //2: en passant, 3: arrocco corto, 4: arrocco lungo
            int moveName;
            //pezzo cattuarto   se moveName = 1,2
            //torre mossa       se moveName = 3,4
            Pieces* additionalPiece;
            //costruttori
            Move(Pieces* p, std::pair<int, int> dest, int name, Pieces* add);
            Move();
        };
        ChessBoard(std::string log = "", std::string playerWhite = "", std::string playerBlack = "");
        //ritorna vettore con tutte le mosse possibili per giocatore con color
        std::vector<Move> movesAvailable(char color);
        //metodo generale
        //ritorna true se è possibile promozione, false altrimenti
        bool performMove(Move move);
        //metodo per giocatore: fornire posizioni
        //ritorna true se è possibile promozione, false altrimenti
        bool performMove(std::pair<int, int>& start, std::pair<int, int>& , char color = 0);
        //metodo per computer: mossa presa automaticamente da nextPlayerMoves
        //ritorna true se è possibile promozione, false altrimenti
        bool performMove();
        //effettua promozione di pedone salvato in pieceToPromote con pezzo indicato da newPiece
        void performPromotion(char newPiece);
        //stampa scacchiera
        std::string printBoard();
        //ritorna numero che rappresenta situazione in cui si trova giocatore con color:
        //0 se in scaccomatto, 1 se scacco, 2 se stallo, 3 se patta per mancanza di pezzi,
        //4 se patta per numero di mosse, 5 se possibile patta per ripetizioni di posizione,
        //-1 altrimenti
        int getCondition(char color);
        //restituisce posizione di pedone che può essere promosso (usato da bot)
        std::pair<int, int> getPawnToPromote();
        //aggiorna log con informazioni su vittoria
        //0 se in scaccomatto, 2 se stallo, 3 se patta per mancanza di pezzi,
        //4 se patta per numero di mosse, 5 se patta per ripetizione di posizione,
        //6 se patta per raggiungimento limite mosse in partita tra bot,
        //7 se patta per accordo
        void updateLogVictory(int ending);
        //eccezione: input non valido
        class InvalidInputException {};
        //eccezione: mossa non valida
        class InvalidMoveException {};
    private:
        //ogni vettore rappresenta una riga
        std::vector<std::vector<Pieces*>> board;
        //prima pezzi bianchi (pedoni per ultimi), poi pezzi neri (pedoni per ultimi)
        std::vector<Pieces*> piecesList;
        //acronimo di piecesLeftWithoutKings
        int PLWK;
        //ultima mossa effettuata
        Move lastMove;
        //nome del file su cui effettuare il log
        std::string logFile;
        //pedone che può essere promossa
        Pieces* pieceToPromote;
        //salva la condizione di giocatore attuale
        int condition = -1;
        //vettore contenente le mosse disponibili al giocatore attuale
        std::vector<Move> nextPlayerMoves;
        //mappa che salva le configurazioni apparse e il numero di apparizioni di ciascuna
        std::map<std::string, int> positions;
        //numero di mosse effettuate senza muovere pedoni e catturare pezzi (per patta per numero di mosse)
        int drawMoves;
        //numero di righe e colonne della scacchiera
        const int SIZE = 8;
        //costruisce pezzi da inserire in riga (configurazione iniziale della scacchiera)
        void initializeRow(int row);
        //inserisce un pezzo in board e piecesList
        void insertPiece(Pieces* piece, std::pair<int, int>* pos);
        //aggiorna log con mossa
        void updateLogMove(std::pair<int, int> start, std::pair<int, int> finish);
        //aggiorna log con pezzo inserito con promozione
        void updateLogPromotion(char newPiece);
        //aggiorna log con scacco
        void updateLogCheck();
        //controlla se input della mossa è valido
        bool legitMoveInput(std::pair<int, int>& x);
        //controlla se posizione/coordinate inserite sono all'interno della scacchiera
        bool scanBoundaries(std::pair<int, int>& pos);
        bool scanBoundaries(int row, int column);
        ///se casella occupata, ritorna colore del pezzo presente
        //altrimenti, ritorna 0
        char scanOccupied(std::pair<int, int>& pos);
        char scanOccupied(int row, int column);
        //controlla se il re con color specificato è in scacco
        //row e column: eventuali coordinate modificate del re
        bool scanCheck(char color, int row = -1, int column = -1);
        //esegue mossa in copia di scacchiera e controlla scacco di re di color specificato
        bool scanCheck(Move& move, char color);
        //controlla se giocatore è in saccomatto: in scacco e senza mosse disponibili
        bool scanCheckmate(bool initialCheck, std::vector<Move>& moves);
        //controlla se mancano pezzi necessari per eseguire scaccomatto
        bool scanCheckmateImpossibility();
        //controlla e aggiunge alle mosse disponibili arrocco, en passant, cattura in diagonale da parte dei pedoni
        void scanAddSpecialMoves(std::vector<Move>& moves, char color);
        //controlla se piece può effettuare promozione
        bool scanPromotion(Pieces* piece);
        //controlla se pezzi specificati possono effettuare en passant
        //p1 = pedone che cattura, p2 = pedone che viene catturato
        bool enPassantConditions(Pieces* p1, Pieces* p2);
        //controlla se è possibile arrocco con pezzi specificati
        //true se possibile arrocco, tipo di arrocco determinato da chiamante
        bool castlingConditions(Pieces* king, Pieces* tower);
        //mappa utilizzata in scanCheck
        //chiave: direzione da cui può essere mangiato il re; valore: std::stringa con pezzi che possono
        //mangiare da quella direzione
        std::map<std::pair<int, int>, std::string> directionsPieces{
            {std::pair<int, int>(1, 1), "PRAD"},
            {std::pair<int, int>(1, 0), "RDT"},
            {std::pair<int, int>(1, -1), "PRAD"},
            {std::pair<int, int>(0, -1), "RDT"},
            {std::pair<int, int>(-1, -1), "PRAD"},
            {std::pair<int, int>(-1, 0), "RDT"},
            {std::pair<int, int>(-1, 1), "PRAD"},
            {std::pair<int, int>(0, 1), "RDT"}};
        //vettore utilizzato in scanCheck
        //posizioni relative (rispetto a re) da cui cavallo può mangiare re
        std::vector<std::pair<int, int>> directionsHorse{
            std::pair<int, int>(1, 2), std::pair<int, int>(2, 1), std::pair<int, int>(2, -1), std::pair<int, int>(1, -2), std::pair<int, int>(-1, -2), std::pair<int, int>(-2, -1), std::pair<int, int>(-2, 1), std::pair<int, int>(-1, 2)};
};
//operatore di confronto per Move
//controlla se pezzo e posizione di arrivo sono uguali
bool operator==(const ChessBoard::Move& m1, const ChessBoard::Move& m2);

#endif