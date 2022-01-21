#include "chessBoard.h"
using namespace std;

ChessBoard::Move::Move(Pieces* p, pair<int, int> dest, int name, Pieces* add = nullptr) : 
piece {p}, destination {dest}, moveName {name}, additionalPiece {add} {}

ChessBoard::Move::Move() : piece {nullptr}, destination {pair<int, int>(-1, -1)}, moveName{-1}, 
additionalPiece {nullptr} {}

bool operator==(const ChessBoard::Move& m1, const ChessBoard::Move& m2) {
    return (m1.piece == m2.piece && m1.destination == m2.destination);
}

ChessBoard::ChessBoard(string log, string playerWhite, string playerBlack) {
    //inizializzare board
    for (int i = 0; i < 8; i++) board.push_back(vector<Pieces*>(8, nullptr));
    //costruire pezzi, inserendoli in board e piecesList
    initializeRow(0);
    initializeRow(1);
    initializeRow(7);
    initializeRow(6);
    //inizializza variabili private (lastMove, nextPlayerMoves, pieceToPromote, logfile, drawMoves, PLWK)
    lastMove = Move();
    nextPlayerMoves.push_back(Move());
    pieceToPromote = nullptr;
    logFile = log;
    drawMoves = 0;
    PLWK = 30;
    //se presente file di log, inserisce info su giocatori in log
    if (log != "" && playerWhite != "" && playerBlack != "") {
        ofstream write(logFile);
        string playerRow = "B: " + playerWhite + "\nN: " + playerBlack + "\n\n";
        write << playerRow;
        write.close();
    }
}

vector<ChessBoard::Move> ChessBoard::movesAvailable(char color) {
    vector<Move> moves;
    //offset di pezzi bianchi/neri in piecesList
    int start = 0;
    if (color == 'N') start = SIZE*2;
    //controlla se giocatore è in scacco
    bool initialCheck = scanCheck(color);
    for (int i = 0; i < SIZE*2; i++) {
        Pieces* piece = piecesList[start+i];
        //se pezzo è stato mangiato, salta iterazione
        if (!piece) continue;
        //ottiene caselle in cui pezzo può teoricamente muoversi
        vector<vector<pair<int, int>>> pieceMoves = piece->Pmove();
        for (int j = 0; j < pieceMoves.size(); j++) {
            vector<pair<int, int>> tmp = pieceMoves[j];
            for (int k = 0; k < tmp.size(); k++) {
                pair<int, int> destination = tmp[k];
                //se casella è fuori da scacchiera, salta iterazione
                if (!scanBoundaries(destination)) continue;
                Pieces* additionalPiece = nullptr;
                int moveName = 0;
                char occ = scanOccupied(destination);
                //se casella è occupata e pezzo che si muove è pedone, terminare ciclo
                //(evitare che pedone possa eseguire mossa da due anche con pezzo in mezzo)
                if (occ != 0 && (piece->GetName() == 80 || piece->GetName() == 112)) break;
                //se casella è occupata da pezzo amico e ci sono più vettori in pieceMoves, saltare al prossimo vettore
                //(ogni vettore rappresenta una direzione in cui pezzo potrebbe muoversi)
                else if (occ == color && pieceMoves.size() > 1) break;
                //se casella è occupata da pezzo amico e c'è un solo vettore in pieceMoves, saltare alla prossima posizione
                //(il vettore rappresenta le posizioni in cui pezzzo potrebbe muoversi)
                else if (occ == color && pieceMoves.size() == 1) continue;
                //se casella è occupata da pezzo avversario, quest'ultimo segnato come pezzo da mangiare
                else if (occ != color && occ != 0) {
                    moveName++;
                    additionalPiece = board[destination.first][destination.second];
                }
                //costruisce mossa con informazioni ottenute
                Move move = Move(piece, destination, moveName, additionalPiece);
                //una mossa è legale se non mette il proprio re sotto scacco
                if (!scanCheck(move, color))
                    moves.push_back(Move(piece, destination, moveName, additionalPiece));
                //condizione indicata in riga 65 ripetuta nel caso in cui casella occupata da pezzo avversario
                //(serve aggiungere mossa prima di passare a prossimo vettore)
                if (occ != 0 && pieceMoves.size() > 1) break;
            }
        }
    }
    //aggiunge mosse rimanenti
    scanAddSpecialMoves(moves, color);
    //se giocatore in scaccomatto, aggiorna condizione
    if (scanCheckmate(initialCheck, moves)) condition = 0;
    //se giocatore in scacco, aggiorna log e condizione
    else if (initialCheck) {
        if (logFile != "") updateLogCheck();
        condition = 1;
    }
    //se giocatore in stallo, aggiorna condizione
    else if (moves.size() == 0) condition = 2;
    //altrimenti, setta condition a valore di default
    else condition = -1;
    return moves;
}

bool ChessBoard::performMove(Move move) {
    //settare pieceToPromote a nullptr (serve se è appena stata fatta una promozione)
    pieceToPromote = nullptr;
    Pieces* piece = move.piece;
    pair<int, int> start = piece->GetPosition();
    pair<int, int> destination = move.destination;
    int name = move.moveName;
    //aggiorna variabili membro del pezzo con nuova posizione
    piece->SetMove(destination);
    //aggiorna board con spostamento
    board[destination.first][destination.second] = piece;
    board[start.first][start.second] = nullptr;
    switch (name) {
        case 0:
            break;
        case 3: {   //arrocco corto
                //additionalPiece in arrocco: torre da spostare
                Pieces* tower = move.additionalPiece;
                pair<int, int> pos = tower->GetPosition();
                //colonna attuale: 7, col. destinazione: 5
                tower->SetMove(pair<int, int>(pos.first, pos.second-2));
                board[pos.first][pos.second] = nullptr;
                board[pos.first][pos.second-2] = tower;
                break;
            }
        case 4: {   //arrocco lungo
                Pieces* tower = move.additionalPiece;
                pair<int, int> pos = tower->GetPosition();
                //colonna attuale: 0, col. destinazione: 4
                tower->SetMove(pair<int, int>(pos.first, pos.second+3));
                board[pos.first][pos.second] = nullptr;
                board[pos.first][pos.second+3] = tower;
                break;
            }
        default:    //mossa normale con cattura, en passant
            Pieces* additionalPiece = move.additionalPiece;
            //rimuove pezzo mangiato da piecesList
            *(find(piecesList.begin(), piecesList.end(), additionalPiece)) = nullptr;
            //se en passant, rimuove pedone mangiato da scacchiera (non è stato rimosso da riga 111)
            if (name == 2) {
                pair<int, int> pawnPos = additionalPiece->GetPosition();
                board[pawnPos.first][pawnPos.second] = nullptr;
            }
            //dealloca memoria riservata a pezzo mangiato
            delete additionalPiece;
            //decrementa contatore di pezzi rimasti
            PLWK--;
            break;
    }
    //se non è stato spostato pedone o mangiato pezzo, aumenta mossa per patta
    //altrimenti, riportarle a zero
    if (piece->GetName() != 'P' && piece->GetName() != 'p' && name != 1 && name != 2) drawMoves++;
    else drawMoves = 0;
    condition = -1;
    lastMove = move;
    //salva apparizione della posizione corrente
    positions[printBoard()]++;
    //se log presente, aggiorna log con mossa
    if (logFile != "") updateLogMove(start, destination);
    //se possibile promozione, salva puntatore a pedone da promuovere
    if (scanPromotion(piece)) {
        pieceToPromote = piece;
        return true;
    }
    return false;
}

bool ChessBoard::performMove(pair<int, int>& start, pair<int, int>& destination) {
    if (!(legitMoveInput(start) && legitMoveInput(destination))) throw InvalidInputException();
    //salva puntatore in casella inserita
    Pieces* piece = board[start.first][start.second];
    //crea mossa con parametri sufficienti per controllo
    Move tmpMove = Move(piece, destination, 0, nullptr);
    //cerca mossa tra quelle disponili
    auto result = find(nextPlayerMoves.begin(), nextPlayerMoves.end(), tmpMove);
    //se mossa assente, lancia eccezione
    if (result == nextPlayerMoves.end()) throw InvalidMoveException();
    return performMove(*result);
}

bool ChessBoard::performMove() {
    //estrae mossa casuale da quelle disponibili
    int i = rand() % nextPlayerMoves.size();
    Move m = nextPlayerMoves[i];
    return performMove(m);
}

void ChessBoard::performPromotion(char code) {
    pair<int, int> pos = pieceToPromote->GetPosition();
    char color = pieceToPromote->GetColor();
    int moves = pieceToPromote->GetStatus();
    Pieces* newPiece;
    switch (code) {
        case 'A':    //alfiere
            newPiece = new A(pos, color, moves);
            break;
        case 'C':    //cavallo
            newPiece = new C(pos, color, moves);
            break;
        case 'D':    //regina
            newPiece = new D(pos, color, moves);
            break;
        case 'T':    //torre
            newPiece = new T(pos, color, moves);
            break;
        default:
            throw InvalidInputException();
    }
    //aggiorna board con nuovo pezzo
    board[pos.first][pos.second] = newPiece;
    //sostituisce pedone con nuovo pezzo in piecesList
    *(find(piecesList.begin(), piecesList.end(), pieceToPromote)) = newPiece;
    //dealloca memoria riservata a pedone
    delete pieceToPromote;
    //se log presente, aggiorna log con promozione
    if (logFile != "") updateLogPromotion(code);
}

string ChessBoard::printBoard() {
    string out = "";
    out += "   ┌───┬───┬───┬───┬───┬───┬───┬───┐\n";
    for (int i = 7; i >= 0; i--) {
        out += to_string(i+1);
        out += "  │ ";
        for (int j = 0; j < 8; j++) {
            if (board[i][j] != nullptr) out += board[i][j]->GetName();
            else out += " ";
            out += " │ ";
        }
        out += "\n";
        if (i >= 1) out += "   ├───┼───┼───┼───┼───┼───┼───┼───┤\n";
    }
    out += "   └───┴───┴───┴───┴───┴───┴───┴───┘\n";
    out += "     A   B   C   D   E   F   G   H\n\n";
    return out;
}

int ChessBoard::getCondition(char color) {
    //se condizione normale oppure scacco, aggiornare mosse per prossimo giocatore
    if (condition == -1 || condition == 1) nextPlayerMoves = movesAvailable(color);
    //se non c'è scaccomatto o stallo (fanno interrompere subito partita), controllare altre condizioni
    if (condition != 0 && condition != 2) {
        try {   //positions.at lancia eccezione out_of_range se stringa non presente in positions
            if (scanCheckmateImpossibility()) condition = 3;
            else if (drawMoves >= 50) condition = 4;
            else if (positions.at(printBoard()) == 3) {
                condition = 5;
                //elimina scacchiera da posizioni apparse
                positions.erase(printBoard());
            }
        }
        catch (out_of_range& e) {}
    }
    return condition;
}

pair<int, int> ChessBoard::getPawnToPromote() {
    return pieceToPromote->GetPosition();
}

void ChessBoard::updateLogVictory(int ending) {
    ofstream write;
    write.open(logFile, ofstream::app);
    write << "END:" << ending;
    write.close();
}

void ChessBoard::initializeRow(int row) {
    char color;
    if (row == 0 || row == 1) color = 'B';
    else color = 'N';
    Pieces* piece;
    if (row == 1 || row == 6) { //riga con pedoni
        for (int i = 0; i < SIZE; i++) {
            piece = new P(pair<int, int>(row, i), color);
            insertPiece(piece, new pair<int, int>(row, i));
        }
    }
    else if (row == 0 || row == 7) {    //riga con altri pezzi
        piece = new T(pair<int, int>(row, 0), color);
        insertPiece(piece, new pair<int, int>(row, 0));
        piece = new C(pair<int, int>(row, 1), color);
        insertPiece(piece, new pair<int, int>(row, 1));
        piece = new A(pair<int, int>(row, 2), color);
        insertPiece(piece, new pair<int, int>(row, 2));
        piece = new D(pair<int, int>(row, 3), color);
        insertPiece(piece, new pair<int, int>(row, 3));
        piece = new R(pair<int, int>(row, 4), color);
        insertPiece(piece, new pair<int, int>(row, 4));
        piece = new A(pair<int, int>(row, 5), color);
        insertPiece(piece, new pair<int, int>(row, 5));
        piece = new C(pair<int, int>(row, 6), color);
        insertPiece(piece, new pair<int, int>(row, 6));
        piece = new T(pair<int, int>(row, 7), color);
        insertPiece(piece, new pair<int, int>(row, 7));
    }
}

void ChessBoard::insertPiece(Pieces* piece, pair<int, int>* pos) {
    board[pos->first][pos->second] = piece;
    piecesList.push_back(piece);
}

void ChessBoard::updateLogMove(pair<int, int> start, pair<int, int> end) {
    ofstream write;
    write.open(logFile, ofstream::app);
    string out;
    out += to_string(start.first) + to_string(start.second) + " ";
    out += to_string(end.first) + to_string(end.second) + "\n";
    write << out;
    write.close();
}

void ChessBoard::updateLogPromotion(char newPiece) {
    ofstream write;
    write.open(logFile, ofstream::app);
    write << "p " << newPiece << "\n";
    write.close();
}

void ChessBoard::updateLogCheck() {
    ofstream write;
    write.open(logFile, ofstream::app);
    write << "c";
    write.close();
}

bool ChessBoard::legitMoveInput(pair<int, int>& x) {
    return (x.first >= 0 && x.first < 8 && x.second >= 0 && x.second < 8);
}

bool ChessBoard::scanBoundaries(pair<int, int>& pos) {
    return (pos.first >= 0 && pos.first < SIZE && pos.second >= 0 && pos.second < SIZE);
}

bool ChessBoard::scanBoundaries(int row, int column) {
    pair<int, int> tmp = pair<int, int>(row, column);
    bool result = scanBoundaries(tmp);
    return result;
}

char ChessBoard::scanOccupied(pair<int, int>& pos) {
    Pieces* piece = board[pos.first][pos.second];
    if (!piece) return 0;
    else return piece->GetColor();
}

char ChessBoard::scanOccupied(int row, int column) {
    pair<int, int> tmp = pair<int, int>(row, column);
    char result = scanOccupied(tmp);
    return result;
}

bool ChessBoard::scanCheck(char color, int row, int column) {
    int offset = 0;
    if (color == 'N') offset = SIZE*2;
    
    pair<int, int> pos;
    //se posizione non specificata, prende posizione attuale del re
    if (row == -1 && column == -1) {
        Pieces* king = piecesList[4+offset];
        pos = king->GetPosition();
    }
    else pos = pair<int, int>(row, column);
    //ciclo su ogni direzione possibile da cui re può essere mangiato
    for (auto it = directionsPieces.begin(); it != directionsPieces.end(); ++it) {
        pair<int, int> direction = it->first;
        string pieces = it->second;
        //posizione da controllare (presenza pezzi)
        pair<int, int> tmp = pair<int, int>(pos.first+direction.first, pos.second+direction.second);
        int i = 1;
        //continua a ispezionare direzione finchè non esce da scacchiera
        while (scanBoundaries(tmp)) {
            char pieceColor = scanOccupied(tmp);
            char pieceName;
            //se casella non è occupata, va avanti
            if (pieceColor != 0) {
                //se casella occupata da pezzo amico, interrompere controllo su direzione
                if (pieceColor == color) break;
                char pieceName = board[tmp.first][tmp.second]->GetName();
                //se lettera del pezzo è minuscola, riportarla a maiuscola
                if (pieceName > 90) pieceName -= 32;
                //cerca lettera di pezzo presente in casella tra pezzi che possono mangiare da quella direzione
                auto searchResult = find(pieces.begin(), pieces.end(), pieceName);
                //se pezzo è presente, presente scacco
                //inoltre, se pezzo presente è pedone oppure re, serve controllare di essersi spostati di uno (altrimenti non può mangiare re)
                if (searchResult != pieces.end() && ((pieceName != 'P' && pieceName != 'R') || 
                    i == 1)) return true;
                
                else break;
            }
            //si sposta in casella successiva in quella direzione
            tmp.first += direction.first;
            tmp.second += direction.second;
            i++;
        }
    }
    //ciclo su ogni casella possibile da cui un cavallo può mangiare re
    for (int i = 0; i < directionsHorse.size(); i++) {
        pair<int, int> direction = directionsHorse[i];
        pair<int, int> tmp = pair<int, int>(pos.first+direction.first, pos.second+direction.second);
        //non ciclo: ogni posizione indicata è unica, non direzione da seguire
        if (scanBoundaries(tmp)) {
            char pieceColor = scanOccupied(tmp);
            if (pieceColor != 0 && pieceColor != color) {
                char pieceName = board[tmp.first][tmp.second]->GetName();
                if (pieceName > 90) pieceName -= 32;
                if (pieceName == 'C') return true;
            }
        }
    }
    return false;
}

bool ChessBoard::scanCheck(Move& move, char color) {
    //salva copia della scacchiera attuale
    vector<vector<Pieces*>> oldBoard = board;
    //esegue mossa senza aggiornare variabili membro di pezzo e deallocare memoria
    Pieces* piece = move.piece;
    pair<int, int> start = move.piece->GetPosition();
    pair<int, int> end = move.destination;
    board[end.first][end.second] = piece;
    board[start.first][start.second] = nullptr;
    switch (move.moveName) {
        case 0:
            break;
        case 2: {
                pair<int, int> pawnPos = move.additionalPiece->GetPosition();
                board[pawnPos.first][pawnPos.second] = nullptr;
                break;
            }
        case 3: {
                Pieces* tower = move.additionalPiece;
                pair<int, int> pos = tower->GetPosition();
                board[pos.first][pos.second] = nullptr;
                board[pos.first][pos.second-2] = tower;
                break;
            }
        case 4: {
                Pieces* tower = move.additionalPiece;
                pair<int, int> pos = tower->GetPosition();
                board[pos.first][pos.second] = nullptr;
                board[pos.first][pos.second+3] = tower;
                break;
            }
    }
    bool result;
    //se pezzo spostato è re, chiamare metodo specificando posizione
    if (piece->GetName() == 'R' || piece->GetName() == 'r') 
        result = scanCheck(color, end.first, end.second);
    else result = scanCheck(color);
    //ripristinare board vecchia
    board = oldBoard;
    return result;
}

bool ChessBoard::scanCheckmate(bool initialCheck, vector<Move>& moves) {
    return initialCheck && moves.size() == 0;
}

bool ChessBoard::scanCheckmateImpossibility() {
    //se presenti più di due pezzi tranne re, possibile scaccomatto
    if (PLWK > 2) return false;
    if (PLWK == 0) return true; //ovvero, presenti solo re: impossibile scaccomatto
    int horsesNumber = 0;
    //cavalli creati all'inizio della partita
    vector<int> horsesIndexes{1, 6, 17, 22};
    for (int i : horsesIndexes) {
        if (piecesList[i]) horsesNumber++;
    }
    //ispeziona pedoni (possibili che siano stati promossi in cavalli)
    for (int i = 0; i < SIZE; i++) {
        Pieces* tmpB = piecesList[8+i];
        Pieces* tmpN = piecesList[24+i];
        if (tmpB && tmpB->GetName() == 'c') horsesNumber++;
        if (tmpN && tmpN->GetName() == 'C') horsesNumber++;
    }
    //se presente solo un cavallo oltre ai re, impossibile scaccomatto
    if (horsesNumber == 1 && PLWK == 1) return true;
    int WLSBN = 0; //alfieri bianchi campochiaro
    int BLSBN = 0; //alfieri neri campochiaro
    int WDSBN = 0; //alfieri bianchi camposcuro
    int BDSBN = 0; //alfieri neri camposcuro
    //controlla presenza pedoni creati all'inizio della partita
    if (piecesList[2]) WDSBN++;
    if (piecesList[5]) WLSBN++;
    if (piecesList[18]) BLSBN++;
    if (piecesList[21]) BDSBN++;
    //ispeziona pedoni (possibili che siano stati promossi in alfieri)
    for (int i = 0; i < SIZE; i++) {
        Pieces* tmpB = piecesList[8+i];
        Pieces* tmpN = piecesList[24+i];
        //se somma coordinate è pari, alfiere si trova su camposcuro
        //altrimenti, su campochiaro
        if (tmpB && tmpB->GetName() == 'a') {
            if (i%2 == 0) WDSBN++;
            else WLSBN++;
        }
        if (tmpN && tmpN->GetName() == 'A') {
            if (i%2 == 0) BDSBN++;
            else BLSBN++;
        }
    }
    //se presenti un solo alfiere oltre a re, impossibile scaccomatto
    if (BDSBN + BLSBN + WDSBN + WLSBN == 1 && PLWK == 1) return true;
    //se presenti solo due alfieri oltre a re e sono su stesso campo, impossibile scaccomatto
    if ((WLSBN == 1 && BLSBN == 1 || WDSBN == 1 && BDSBN == 1) && PLWK == 2) return true;
    return false;
}

void ChessBoard::scanAddSpecialMoves(vector<Move>& moves, char color) {
    int offset = 0;
    if (color == 'N') offset = SIZE*2;

    //arrocco
    Pieces* firstTower = piecesList[0+offset];
    Pieces* secondTower = piecesList[7+offset];
    Pieces* king = piecesList[4+offset];
    int row = (color == 'B') ? 0 : 7;
    //arrocco lungo
    if (castlingConditions(king, firstTower))
        moves.push_back(Move(king, pair<int, int>(row,2), 4, firstTower));
    //arrocco corto
    if (castlingConditions(king, secondTower))
        moves.push_back(Move(king, pair<int, int>(row,6), 3, secondTower));

    //en passant + controllare se pedoni possono mangiare in diagonale
    int direction;
    if (color == 'B') direction = 1;
    else direction = -1;
    Move tmp;
    //controlla tutti i pedoni
    for (int i = 0; i < SIZE; i++) {
        Pieces* pawn = piecesList[8+i+offset];
        //se pedone non esiste, salta
        if (!pawn) continue;
        //se pedone è stato promosso, salta
        if (pawn->GetName() != 80 && pawn->GetName() != 112) continue;
        pair<int, int> pos = pawn->GetPosition();
        //controlla che casella alla sinistra del pezzo sia dentro alla scacchiera
        if (scanBoundaries(pos.first, pos.second-direction)) {
            //costruisce mossa en passant
            Pieces* toTheLeft = board[pos.first][pos.second-direction];
            pair<int, int> destination = pair<int, int>(pos.first + direction, pos.second - direction);
            tmp = Move(pawn, destination, 2, toTheLeft);
            //controlla se è possibile en passant e se non mette re in scacco
            if (enPassantConditions(pawn, toTheLeft) && !scanCheck(tmp, color))
                moves.push_back(tmp);
        }
        //controlla che casella alla dstra del pezzo sia dentro alla scacchiera
        if (scanBoundaries(pos.first, pos.second+direction)) {
            Pieces* toTheRight = board[pos.first][pos.second+direction];
            pair<int, int> destination = pair<int, int>(pos.first + direction, pos.second + direction);
            tmp = Move(pawn, destination, 2, toTheRight);
            if (enPassantConditions(pawn, toTheRight) && !scanCheck(tmp, color))
                moves.push_back(tmp);
        }
        //controlla che casella davanti a sinistra del pezzo sia dentro alla scacchiera
        if (scanBoundaries(pos.first+direction, pos.second-direction)) {
            //costruisce mossa con cattura
            Pieces* forwardLeft = board[pos.first+direction][pos.second-direction];
            pair<int, int> destination = pair<int, int>(pos.first + direction, pos.second - direction);
            tmp = Move(pawn, destination, 1, forwardLeft);
            //controlla se esiste pezzo in casella, se è un pezzo avversario e se non mette proprio re in scacco
            if (forwardLeft && forwardLeft->GetColor() != color && !scanCheck(tmp, color))
                moves.push_back(tmp);
        }
        //controlla che casella davanti a destra del pezzo sia dentro alla scacchiera
        if (scanBoundaries(pos.first+direction, pos.second+direction)) {
            Pieces* forwardRight = board[pos.first+direction][pos.second+direction];
            pair<int, int> destination = pair<int, int>(pos.first + direction, pos.second + direction);
            tmp = Move(pawn, destination, 1, forwardRight);
            if (forwardRight && forwardRight->GetColor() != color && !scanCheck(tmp, color))
                moves.push_back(tmp);
        }
    }
}

bool ChessBoard::scanPromotion(Pieces* piece) {
    //controlla se pezzo specificato è pedone e se è in riga giusta
    return (piece->GetName() == 'P' && piece->GetPosition().first == 0 ||
        piece->GetName() == 'p' && piece->GetPosition().first == 7);
}

bool ChessBoard::enPassantConditions(Pieces* p1, Pieces* p2) {
    if (!p2) return false;  //controlla se esiste pezzo
    char n1 = p1->GetName();
    char n2 = p2->GetName();
    int row2 = p2->GetPosition().first;
    int row1 = p1->GetPosition().first;
    //controlla se pezzo che cattura è pedone nero in riga 5 oppure pedone bianco in riga 4,
    //se pezzo da catturare è ultimo pezzo mosso, se è stato mosso per la prima volta,
    //e se entrambi i pezzi sono pedoni
    return ((n2 == 'P' && row2 == 4 || n2 == 'p' && row2 == 3) &&
        lastMove.piece == p2 && p2->GetStatus() == 1 && n1-n2 != 0);
}

bool ChessBoard::castlingConditions(Pieces* king, Pieces* tower) {
    //se uno dei pezzi non esiste, esce
    if (!(king && tower)) return 0;
    //se uno dei pezzi è già stato mosso, esce
    if (!(king->GetStatus() == 0 && tower->GetStatus() == 0)) return 0;
    int start = king->GetPosition().second;
    int finish = tower->GetPosition().second;
    int row = king->GetPosition().first;
    int tmp = tower->GetPosition().first;
    char color = king->GetColor();
    //rappresenta direzione in cui muoversi (fattore con cui modificare posizione)
    int factor;
    if (start < finish) factor = 1;
    else factor = -1;
    //se scacco attualmente presente, impossibile eseguirlo
    if (scanCheck(color)) return false;
    for (int i = 1; i < abs(start-finish); i++) {
        //costruisce mossa temporanea
        Move tmp = Move(king, pair<int, int>(row, start + i*factor), 0);
        //se casella è occupata da altro pezzo e se re attraversa casella in cui è in scacco, impossibile arrocco
        if (scanOccupied(row, start + i*factor) != 0 || (i <= 2 && scanCheck(tmp, color)))
            return false;
    }
    return true;
}