#include "chessBoard.h"
#include <thread>
#include <chrono>
#include <iostream>
using namespace std;

//acronimo di printTextEffect (standard: 1,250)
void PTE(std::string s, int delayShort = 1, int delayLong = 250) {
    for (int i = 0; i < s.size(); i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(delayShort));
        std::cout << s[i];
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(delayLong));
    std::cout << std::endl;
}

int main(int argc, char** args)
 {
    if (argc <= 2)
     {
        PTE("Non sono stati inseriti gli argomenti necessari; rieseguire il programma.");
        return 0;
    }

    char op = *args[1];
    if (!(op == 'v' || op == 'f')) 
    {
        PTE("Il codice per l'operazione inserito non è valido; rieseguire il programma.");
        return 0;
    }

    if (op == 'f' && argc == 3)
     {
        PTE("Gli argomenti inseriti non sono sufficienti per il replay su file; rieseguire il programma.");
        return 0;
    }

    if (op == 'v' && argc > 3)
     {
        PTE("Sono stati inseriti troppi argomenti per il replay a video; rieseguire il programma.");
        return 0;
    }

    if (op == 'f' && argc > 4)
     {
        PTE("Sono stati inseriti troppi argomenti per il replay su file; rieseguire il programma.");
        return 0;
    }

    string logFile (args[2]);
    string replayFile;
    if (argc >= 4) replayFile = args[3];
    //se il nome del file non contiene l'estensione, viene aggiunta

    if (logFile.size() < 5 || logFile.substr(logFile.size()-4) != ".txt") logFile += ".txt";
    ifstream scanner (logFile);
    //controlla se file di log inserito esiste

    if (!scanner.good())
     {
        PTE("Il file di log inserito non esiste; rieseguire il programma.");
        return 0;
    }

    PTE("Benvenuto nel replay delle partite di scacchi!");
    //stringa usata per salvare i messaggi da stampare nel corso del programma
    string message;
    message = "Verrà effettuato il replay ";
    message += (op == 'v') ? "a video " : "sul file '" + replayFile + "' ";
    message += "della partita salvata sul file di log '" + logFile + "'!";
    PTE(message);
    //ottiene nomi giocatori (prime due righe del log)
    string scannerLine;
    getline (scanner, scannerLine);
    string playerWhite = scannerLine.substr(3);
    getline (scanner, scannerLine);
    string playerBlack = scannerLine.substr(3);
    int i = -1;
    char color;
    string promotionPos; 
    //se viene effettuato replay su file, viene aperto oggetto per scrivere su file
    ofstream replayWrite;

    if (op == 'f')
     {
        if (replayFile.size() < 5 || replayFile.substr(replayFile.size()-4) != ".txt") replayFile += ".txt";
        replayWrite.open(replayFile);
    }

    ChessBoard board = ChessBoard();
    int movesNumber = 0;

    while (getline(scanner, scannerLine)) 
    {
        color = (i%2 == 0) ? 'B' : 'N';
        string message;
        bool pb = true;
        //terza riga: riga vuota, nessuna mossa
        if (i == -1)

         {
            pb = false;
            message = "La partita inizia: " + playerWhite + " con le pedine bianche e ";
            message += playerBlack + " con le pedine nere!";
        }

        //ultima riga del log: END: [numero che identifica finale]
        else if (scannerLine.substr(0, 4) == "END:")
         {
            pb = false;
            int ending = scannerLine[4] - 48;
            switch (ending) 
            {
                case 0: //scaccomatto
                    message = (color == 'B') ? playerWhite : playerBlack;
                    message += " è in scaccomatto, ";
                    message += (color == 'B') ? playerBlack : playerWhite;
                    message += " vince!";
                    break;
                case 2: //stallo
                    message = "È stato raggiunto uno stallo! La partita termina!";
                    break;
                case 3: //patta per mancanza di pezzi
                    message = "La partita termina in patta! Non ci sono abbastanza pezzi per eseguire uno scaccomatto!";
                    break;
                case 4: //patta per 50 mosse
                    message = "La partita termina in patta! Sono state eseguite 50 mosse senza spostare pedoni o mangiare pezzi!";
                    break;
                case 5: //patta per ripetizione di posizione
                    message = (color == 'B') ? playerWhite : playerBlack;
                    message += " dichiara la patta dopo che la stessa posizione si è presentata per la terza volta!";
                    break;
                case 6: //patta per superamento limite di mosse in partita tra bot
                    message = "La partita termina in patta! È stato raggiunto il limite di mosse possibili per una partita tra bot!";
                    break;
                case 7: //patta per accordo
                    message = "I giocatori si accordano per la patta! La partita termina!";
                    break;
            }

            //stampa numero totale di mosse
            message += "\nSono state effettuate " + to_string(movesNumber) + " mosse in totale!";
        }

        //promozione effettuata: p [carattere che identifica nuovo pezzo]
        else if (scannerLine.size() == 3)
         {
            //viene decrementato l'indice che identifica il giocatore
            color = (--i%2 == 0) ? 'B' : 'N';
            //ottiene carattere che identifica il nuovo pezzo
            char promotionCode = scannerLine[2];
            //effettua promozione su scacchiera
            board.performPromotion(promotionCode);
            message = (color == 'B') ? playerWhite : playerBlack;
            message += " promuove il pedone in " + promotionPos + " in";
            switch (promotionCode)
             {
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

        //mossa regolare: c[riga di partenza][colonna di partenza] [riga di arrivo][colonna di arrivo]
        //(numerate 0-7) (c presente se giocatore corrente è sotto scacco)
        else {
            bool check = false;
            //controlla se è presente scacco
            if (scannerLine[0] == 'c') 
            {
                check = true;
                scannerLine = scannerLine.substr(1);
            }

            //ottiene coordinate di partenza e arrivo
            char startRow = scannerLine[0];
            char startColumn = scannerLine[1];
            char endRow = scannerLine[3];
            char endColumn = scannerLine[4];
            //crea pair che indicano posizioni (verranno usate in performMove)
            pair<int, int> start = pair<int,int>(startRow-'0', startColumn-'0');
            pair<int, int> end = pair<int, int>(endRow - '0', endColumn - '0');
            //crea stringhe che rappresentano posizioni
            string startString;
            startString.push_back(static_cast<char>(startColumn + 17));
            startString.push_back(static_cast<char>(startRow + 1));
            string endString;
            endString.push_back(static_cast<char>(endColumn + 17));
            endString.push_back(static_cast<char>(endRow + 1));
            message = "Turno di " + ((color == 'B') ? playerWhite : playerBlack);

            if (check) message += " (è sotto scacco!)";
            message += ": " + startString + " in " + endString + "!";
            //se performMove ritorna true, promozione possibile: salva posizione del pedone

            if (board.performMove(start,end,color)) promotionPos = endString;
            //incrementa numero di mosse effettuate
            movesNumber++;
        }
        i++;
        //se replay a video, stampa messaggio costruito e scacchiera attuale a video
        if (op == 'v') 
        {
            PTE(message);
            if (pb) {
                cout << board.printBoard();
                this_thread::sleep_for(chrono::seconds(3));
            }
        }

        //se replay su file, scrive messaggio costruito e scacchiera attuale sul file
        else {
            replayWrite << message << "\n";
            if (pb) replayWrite << board.printBoard() << "\n\n";
        }
    }
    
    if (op == 'f') PTE("Replay effettuato su " + replayFile + "!");
    PTE("Replay finito!");
}