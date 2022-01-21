#include "chessBoard.h"
#include "Gamers.h"
//#include "PTE.cpp"
#include <iostream>
//#include <thread>
//#include <chrono>
using namespace std;

/*acronimo di printTextEffect (standard: 1,250)
void PTE(std::string s, int delayShort = 1, int delayLong = 250) {
    for (int i = 0; i < s.size(); i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(delayShort));
        std::cout << s[i];
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(delayLong));
    std::cout << std::endl;
}*/

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
        cout << "Non è stato inserito l'argomento necessario; rieseguire il prorgramma." << endl;
        return 0;
    }
    if (argc > 2) {
        cout << "Sono stati inseriti troppi argomenti; rieseguire il prorgramma." << endl;
        return 0;
    }
    string game = args[1];
    if (!(game == "pc" || game == "cc")) {
        cout << "L'argomento inserito non è valido; rieseguire il programma." << endl;
        return 0;
    }
    const char B = 66;
    const char U = 85;
    string message = "Benvenuto nel gioco degli scacchi! Verrà effettuata una partita ";
    message += (game == "pc") ? "tra giocatore umano e bot!" : "tra due bot!";
    cout << message;
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
        cout << "Inserisci il nome del giocatore umano: " << endl;
        cin >> names[0];
        //se il nome inserito è presente tra i nomi scelti per i bot, eliminarlo dalla lista
        auto tmp = find(botNames.begin(), botNames.end(), names[0]);
        if (tmp != botNames.end()) botNames.erase(tmp);
        cout << "Giocherai con le pedine bianche!" << endl;
        cout << "Se vuoi proporre una patta, inserisci 'patta' quando ti viene chiesto se vuoi stampare la scacchiera!" << endl;
        names[1] = botNames[rand() % botNames.size()];
        cout << "Il bot si chiama " + names[1] + " e giocherà con le pedine nere." << endl;
        types.push_back(U);
        types.push_back(B);
    }
    else if (game == "pc" && whiteCode == 1) {
        cout << "Inserisci il nome del giocatore umano: " << endl;
        cin >> names[1];
        auto tmp = find(botNames.begin(), botNames.end(), names[0]);
        if (tmp != botNames.end()) botNames.erase(tmp);
        cout << "Giocherai con le pedine nere!" << endl;
        cout << "Se vuoi proporre una patta, inserisci 'patta' quando ti viene chiesto se vuoi stampare la scacchiera!" << endl;
        names[0] = botNames[rand() % botNames.size()];
        cout << "Il bot si chiama " + names[0] + " e giocherà con le pedine bianche." << endl;
        types.push_back(B);
        types.push_back(U);
    }
    else if (game == "cc") {
        int nameIndex = rand() % 10;
        names[0] = botNames[nameIndex];
        //eliminare nome estratto per primo bot da lista di nomi
        botNames.erase(botNames.begin()+nameIndex);
        names[1] = botNames[rand() % 9];
        cout << "Il bot che giocherà con le pedine bianche si chiama " + names[0] + "." << endl;
        cout << "Il bot che giocherà con le pedine nere si chiama " + names[1] + "." << endl;
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
                cout << names[index] + " è in scaccomatto, " + names[(i+1)%2] + " vince! Ecco la scacchiera finale:" << endl;
                cout << board.printBoard();
                endgame = true;
                continue;
            case 1: //scacco
                message += " È sotto scacco!";
                break;
            case 2: //stallo
                cout << "È stato raggiunto uno stallo! Ecco la scacchiera finale:" << endl;
                cout << board.printBoard();
                endgame = true;
                continue;
            case 3: //patta per mancanza di pezzi
                cout << "La partita termina in patta! Non ci sono abbastanza pezzi per eseguire uno scaccomatto! Ecco la scacchiera finale:" << endl;
                cout << board.printBoard();
                endgame = true;
                continue;
            case 4: //patta per 50 mosse
                cout << "La partita termina in patta! Sono state eseguite 50 mosse senza spostare pedoni o mangiare pezzi! Ecco la scacchiera finale:" << endl;
                cout << board.printBoard();
                endgame = true;
                continue;
            case 5: //possibile patta per ripetizione di posizione
                if (game == "pc") { //umano-bot: umano decide se dichiarare patta
                    message = "La configurazione attuale della scacchiera è comparsa per la terza volta! ";
                    //whiteCode rappresenta indice di giocatore umano in names
                    message += names[whiteCode] + ", vuoi dichiarare patta?";
                    cout << message << endl;
                    char draw;
                    cin >> draw;
                    if (draw == 'y') {
                        cout << "La partita termina in patta!" << endl;
                        endgame = true;
                    }
                    else {
                        cout << "Si continua a giocare!" << endl;
                    }
                    continue;
                }
                else {  //bot-bot: decisione casuale su interruzione partita
                    message = "La configurazione attuale della scacchiera è comparsa per la terza volta! ";
                    message += names[index] + " può dichiarare patta!";
                    cout << message;
                    if (randomDecision(BRDD)) {
                        cout << names[index] + " dichiara patta! La partita termina!" << endl;
                        endgame = true;
                    }
                    else {
                        cout << names[index] + " decide di continuare a giocare!" << endl;
                    }
                    continue;
                }
        }
        cout << message << endl;
        Gamers* currentPlayer = players[index];
        if (types[index] == 'U') {  //giocatore umano
            cout << "Se vuoi stampare la scacchiera, inserire 'y': " << endl;
            string code;
            cin >> code;
            if (code == "y") cout << board.printBoard();
            else if (code == "patta") { //se umano propone patta, bot decide casualmente se accettarla
                if (randomDecision(BDAP)) {
                    cout << names[(i+1)%2] + " accetta la patta! La partita termina!" << endl;
                    cond = 7;
                    endgame = true;
                    continue;
                }
                else {
                    cout << names[(i+1)%2] + " non accetta la patta! La partita continua!" << endl;
                }
            }
            string start;
            string end;
            bool result;
            insertMove:
            cout << "Inserisci le coordinate del pezzo che vuoi spostare e della casella nella quale vuoi spostare il pezzo: " << endl;
            cin >> start >> end;
            try {result = currentPlayer->Move(start, end);}
            catch (ChessBoard::InvalidMoveException e) {    //mossa inserita non ammessa
                cout << "La mossa inserita non è valida." << endl;
                goto insertMove;    //ritorna all'inserimento della mossa
            }
            catch (ChessBoard::InvalidInputException e) {   //input non valido
                cout << "L'input inserito non è valido." << endl;
                goto insertMove;    //ritorna all'inserimento della mossa
            }
            if (result) {   //promozione possibile
                message = "Inserisci il pezzo in cui vuoi promuovere il pedone in ";
                pair<int, int> pos = board.getPawnToPromote();
                message += pos.second + 65; //incrementato di 65 per arrivare a lettera che rappresenta colonna
                message += pos.first + 49;  //incrementato di 49 per arrivare a numero che rappresenta riga
                promotion:
                message += "(sia maiuscolo che minuscolo):\n- A: alfiere;\n- C: cavallo;\n- D: regina;\n- T: torre.";
                cout << message << endl;
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
                    cout << message << endl;
                    char answer;
                    cin >> answer;
                    if (answer == 'y') {
                        cout << "Patta accettata! La partita termina!" << endl;
                        endgame = true;
                        cond = 7;
                        continue;
                    }
                    else cout << "La partita continua!" << endl;
                }
                else {  //avversario bot: decide randomicamente se accettare patta
                    cout << message << endl;
                    if (randomDecision(BDAP)) {
                        cout << names[(i+1)%2] + " accetta la patta! La partita termina!" << endl;
                        endgame = true;
                        cond = 7;
                        continue;
                    }
                    else {
                        cout << names[(i+1)%2] + " non accetta la patta! La partita continua!" << endl;
                    }
                }
            }
            bool result = currentPlayer->Move(); //eseguita mossa casuale
            cout << "Mossa effettuata!" << endl;
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
        cout << "La partita termina in patta! È stata effettuata la " + to_string(movesThreshold) + "esima mossa totale!" << endl;
        cond = 6;
    }
    board.updateLogVictory(cond);
    cout << "Grazie per aver giocato!" << endl; 
}