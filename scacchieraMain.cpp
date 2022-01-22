/**
 * @file scacchieraMain.cpp
 * @author Luca Fantin
 */

#include "chessBoard.h"
#include "Gamers.h"
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

//acronimo di printTextEffect (standard: 1,250)
void PTE(std::string s, int delayShort = 1, int delayLong = 250) {
    for (int i = 0; i < s.size(); i++) {
        this_thread::sleep_for(chrono::milliseconds(delayShort));
        cout << s[i];
    }
    this_thread::sleep_for(chrono::milliseconds(delayLong));
    cout << endl;
}

//genera un nome per il file di log: log[primo numero disponibile tra file di log presenti in cartella logs].txt
string CLN() {
    int i = -1;
    ifstream scanner;
    scanner.open("blank.txt");
    string name;
    while (true) {
        name = "logs/log" + to_string(++i) + ".txt";
        scanner.close();
        scanner.open(name);
        if (!scanner.good()) break;
    }
    return name;
}

//prende una decisione casuale (probabilità di decisione true: 1/probability)
bool randomDecision(int probability) {
    return (rand()%probability == 1) ? 1 : 0;
}

int main(int argc, char** args) {
    //inizializza il generatore di numeri random
    srand(time(NULL));
    //controlla validità degli argomenti inseriti
    if (argc < 2) {
        PTE("Non è stato inserito l'argomento necessario; rieseguire il prorgramma.");
        return 0;
    }
    if (argc > 2) {
        PTE("Sono stati inseriti troppi argomenti; rieseguire il prorgramma.");
        return 0;
    }
    string game = args[1];
    if (!(game == "pc" || game == "cc")) {
        PTE("L'argomento inserito non è valido; rieseguire il programma.");
        return 0;
    }
    const char B = 66;
    const char U = 85;
    string message = "Benvenuto nel gioco degli scacchi! Verrà effettuata una partita ";
    message += (game == "pc") ? "tra giocatore umano e bot!" : "tra due bot!";
    PTE(message);
    //vettori che memorizzeranno nome, tipo e oggetti Players dei due giocatori
    //tipo: U per giocatore umano, B per bot
    //in tutti i vettori, giocatore bianco, poi giocatore nero
    vector<string> names(2);
    vector<char> types;
    vector<Gamers*> players;
    vector<string> botNames{"Magnus", "Morphy", "Polgar", "Kasparov", "Gallo", "Shaldon", "Marius", "Bobbi", "Turing", "Pippo"};
    //se white == 0, primo giocatore inserito (giocatore oppure computer) ha il bianco
    //se white == 1, secondo giocatore inserito (computer) ha il bianco
    int whiteCode = randomDecision(2);
    if (game == "pc" && whiteCode == 0) {
        PTE("Inserisci il nome del giocatore umano: ");
        cin >> names[0];
        //se il nome inserito è presente tra i nomi scelti per i bot, eliminarlo dalla lista
        auto tmp = find(botNames.begin(), botNames.end(), names[0]);
        if (tmp != botNames.end()) botNames.erase(tmp);
        PTE("Giocherai con le pedine bianche!");
        PTE("Se vuoi proporre una patta, inserisci 'patta' quando ti viene chiesto se vuoi stampare la scacchiera!");
        names[1] = botNames[rand() % botNames.size()];
        PTE("Il bot si chiama " + names[1] + " e giocherà con le pedine nere.");
        types.push_back(U);
        types.push_back(B);
    }
    else if (game == "pc" && whiteCode == 1) {
        PTE("Inserisci il nome del giocatore umano: ");
        cin >> names[1];
        auto tmp = find(botNames.begin(), botNames.end(), names[0]);
        if (tmp != botNames.end()) botNames.erase(tmp);
        PTE("Giocherai con le pedine nere!");
        PTE("Se vuoi proporre una patta, inserisci 'patta' quando ti viene chiesto se vuoi stampare la scacchiera!");
        names[0] = botNames[rand() % botNames.size()];
        PTE("Il bot si chiama " + names[0] + " e giocherà con le pedine bianche.");
        types.push_back(B);
        types.push_back(U);
    }
    else if (game == "cc") {
        int nameIndex = rand() % 10;
        names[0] = botNames[nameIndex];
        //eliminare nome estratto per primo bot da lista di nomi
        botNames.erase(botNames.begin()+nameIndex);
        names[1] = botNames[rand() % 9];
        PTE("Il bot che giocherà con le pedine bianche si chiama " + names[0] + ".");
        PTE("Il bot che giocherà con le pedine nere si chiama " + names[1] + ".");
        types.push_back(B);
        types.push_back(B);
    }
    //genera scacchiera con nome di file di log e nomi dei giocatori
    ChessBoard board = ChessBoard(CLN(), names[0], names[1]);
    //genera oggetti giocatori e li inserisci in players
    players.push_back(new Gamers('B', &board, names[0], types[0]));
    players.push_back(new Gamers('N', &board, names[1], types[1]));
    int i = 0; //contatore di mosse
    int index; //index = i%2: serve a determinare se è il turno del bianco o del nero
    //limite di mosse generale: 150 se game == "cc", -1 altrimenti (non deve esserci in partita umano-bot)
    int movesThreshold = (game == "cc") ? 150 : -1;
    bool endgame = false; //variabile che dice se partita deve essere interrotta
    int cond = 0; //variabile che identifica la condizione in cui si trova la partita
    constexpr int BDPP = 169; //prob. che bot proponga patta all'avversario (1/valore)
    constexpr int BDAP = 20; //prob. che bot accetti patta proposta dall'avversario (1/valore)
    constexpr int BRDD = 1000; //prob. che bot dichiari patta per ripetizioni di posizione (1/valore)
    while (i != movesThreshold && !endgame) {
        int index = i%2;
        string message = "Turno di " + names[index] + " con le pedine ";
        if (index == 0) message += "bianche!";
        else message += "nere!";
        //ottiene condizione attuale di scacchiera attraverso giocatore, che fornisce il colore del giocatore corrente
        cond = players[index]->GetCondition();
        switch (cond) {
            case 0: //scaccomatto
                PTE(names[index] + " è in scaccomatto, " + names[(i+1)%2] + " vince! Ecco la scacchiera finale:");
                cout << board.printBoard();
                endgame = true;
                continue;
            case 1: //scacco
                message += " È sotto scacco!";
                break;
            case 2: //stallo
                PTE("È stato raggiunto uno stallo! Ecco la scacchiera finale:");
                cout << board.printBoard();
                endgame = true;
                continue;
            case 3: //patta per mancanza di pezzi
                PTE("La partita termina in patta! Non ci sono abbastanza pezzi per eseguire uno scaccomatto! Ecco la scacchiera finale:");
                cout << board.printBoard();
                endgame = true;
                continue;
            case 4: //patta per 50 mosse
                PTE("La partita termina in patta! Sono state eseguite 50 mosse senza spostare pedoni o mangiare pezzi! Ecco la scacchiera finale:");
                cout << board.printBoard();
                endgame = true;
                continue;
            case 5: //possibile patta per ripetizione di posizione
                if (game == "pc") { //umano-bot: umano decide se dichiarare patta
                    message = "La configurazione attuale della scacchiera è comparsa per la terza volta! ";
                    //whiteCode rappresenta indice di giocatore umano in names
                    message += names[whiteCode] + ", vuoi dichiarare patta?";
                    PTE(message);
                    char draw;
                    cin >> draw;
                    if (draw == 'y') {
                        PTE("La partita termina in patta!");
                        endgame = true;
                    }
                    else {
                        PTE("Si continua a giocare!");
                    }
                    continue;
                }
                else {  //bot-bot: decisione casuale su interruzione partita
                    message = "La configurazione attuale della scacchiera è comparsa per la terza volta! ";
                    message += names[index] + " può dichiarare patta!";
                    PTE(message);
                    if (randomDecision(BRDD)) {
                        PTE(names[index] + " dichiara patta! La partita termina!");
                        endgame = true;
                    }
                    else {
                        PTE(names[index] + " decide di continuare a giocare!");
                    }
                    continue;
                }
        }
        PTE(message);
        Gamers* currentPlayer = players[index];
        if (types[index] == 'U') {  //giocatore umano
            PTE("Se vuoi stampare la scacchiera, inserire 'y': ");
            string code;
            cin >> code;
            if (code == "y") cout << board.printBoard();
            else if (code == "patta") { //se umano propone patta, bot decide casualmente se accettarla
                if (randomDecision(BDAP)) {
                    PTE(names[(i+1)%2] + " accetta la patta! La partita termina!");
                    cond = 7;
                    endgame = true;
                    continue;
                }
                else {
                    PTE(names[(i+1)%2] + " non accetta la patta! La partita continua!");
                }
            }
            string start;
            string end;
            bool result;
            insertMove:
            PTE("Inserisci le coordinate del pezzo che vuoi spostare e della casella nella quale vuoi spostare il pezzo: ");
            cin >> start >> end;
            try {result = currentPlayer->Move(start, end);}
            catch (ChessBoard::InvalidMoveException e) {    //mossa inserita non ammessa
                PTE("La mossa inserita non è valida.");
                goto insertMove;    //ritorna all'inserimento della mossa
            }
            catch (ChessBoard::InvalidInputException e) {   //input non valido
                PTE("L'input inserito non è valido.");
                goto insertMove;    //ritorna all'inserimento della mossa
            }
            if (result) {   //promozione possibile
                message = "Inserisci il pezzo in cui vuoi promuovere il pedone in ";
                pair<int, int> pos = board.getPawnToPromote();
                message += pos.second + 65; //incrementato di 65 per arrivare a lettera che rappresenta colonna
                message += pos.first + 49;  //incrementato di 49 per arrivare a numero che rappresenta riga
                promotion:
                message += "(sia maiuscolo che minuscolo):\n- A: alfiere;\n- C: cavallo;\n- D: regina;\n- T: torre.";
                PTE(message);
                char code;
                cin >> code;
                try {board.performPromotion(code);}
                catch (ChessBoard::InvalidInputException e) {   //lettera inserita non valida
                    message = "L'input non è valido, inserisci uno di questi pezzi ";
                    goto promotion; //ritorna a inserimento pezzo
                }
            }
        }
        else {  //giocatore bot
            if (randomDecision(BDPP)) { //decide randomicamente se proporre patta
                message = names[index] + " propone la patta!";
                if (game == "pc") { //avversario umano: viene chiesto se accetta la patta
                    message += "  Vuoi accettare? ";
                    PTE(message);
                    char answer;
                    cin >> answer;
                    if (answer == 'y') {
                        PTE("Patta accettata! La partita termina!");
                        endgame = true;
                        cond = 7;
                        continue;
                    }
                    else PTE("La partita continua!");
                }
                else {  //avversario bot: decide randomicamente se accettare patta
                    PTE(message, 1, 250);
                    if (randomDecision(BDAP)) {
                        PTE(names[(i+1)%2] + " accetta la patta! La partita termina!");
                        endgame = true;
                        cond = 7;
                        continue;
                    }
                    else {
                        PTE(names[(i+1)%2] + " non accetta la patta! La partita continua!");
                    }
                }
            }
            bool result = currentPlayer->Move(); //eseguita mossa casuale
            PTE("Mossa effettuata!");
            if (result) {   //promozione posssibile
                pair<int, int> pos = board.getPawnToPromote();
                char newPiece = players[index]->PerformPromotion(); //pedone promosso in pezzo casuale
                message = "Promozione effettuata: il pedone in ";
                message += pos.second + 65;
                message += pos.first + 49;
                message += " diventa ";
                switch (newPiece) {
                    case 'A':
                        message += " un alfiere!";
                        break;
                    case 'C':
                        message += " un cavallo!";
                        break;
                    case 'D':
                        message += " una regina!";
                        break;
                    case 'T':
                        message += " una torre!";
                        break;
                }
            }
        }
        i++;
    }
    //aggiorna log con informazioni su vittoria
    if (game == "cc" && i == movesThreshold) {
        PTE("La partita termina in patta! È stata effettuata la " + to_string(movesThreshold) + "esima mossa totale!");
        cond = 6;
    }
    board.updateLogVictory(cond);
    PTE("Grazie per aver giocato!"); 
}