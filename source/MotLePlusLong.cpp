//============================================================================
// Name        : MotLePlusLong.cpp
// Author      : Thibaut DA SILVA
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <cstdlib>

using namespace std;

#define CRED        "\033[31m"
#define CDEFAULT    "\033[0m"
#define CBOLD    "\033[1m"

char gVoyelles[] = {'A', 'E', 'I', 'O', 'U', 'Y'};
char gConsonnes[] = {'B','C','D','F','G','H','J','K','L','M','N','P','Q','R','S','T','V','W','X','Z'};

//-------------------------------------------------------------------------

char fnTirage(bool isVoyelle)
{
    srand(time(0));

    char c;
    int i = 0;

    if(isVoyelle)
    {
        i = rand()%5;
        c = gVoyelles[i];
    }
    else
    {
        i = rand()%19;
        c = gConsonnes[i];
    }

    return c;
}

//-------------------------------------------------------------------------

void fnFlush()
{
    int ch;
    while ((ch=getchar()) != EOF && ch != '\n'); //Flush de stdin
}

//-------------------------------------------------------------------------

string fnPropositionUsr(const string lettresTirees)
{
    string proposition;
    bool bOk = false;

    string tmpLettres = lettresTirees;

    while(not bOk)
    {
        //Récupération de la proposition
        cout << endl << "Entrez votre proposition :" << endl;
        cin >> proposition;

        //Parcours de la proposition pour comparer avec les lettres tirées
        for(unsigned int i = 0; i < proposition.length(); i++)
        {
            bOk = false;
            proposition[i] = tolower(proposition[i]);

            for(int j = 0; j < 9; j++)
            {
                //La lettre matche donc on veille à retirer celle du tirage pour éviter les doublons
                if(proposition[i] == tmpLettres[j])
                {
                    tmpLettres[j] = '_';
                    bOk = true;
                    break;
                }
            }

            //Erreur dans la proposition
            if(not bOk)
            {
                cout << CRED  << "Pas de '"<< proposition[i] << "' disponible dans la liste ! Retentez votre chance !" << CDEFAULT << endl;
                break;
            }
        }
    }

    //On vide l'input
    fnFlush();

    return proposition;
}

//-------------------------------------------------------------------------

void fnRechercheDico(const char* filename, const string proposition, const string tirage)
{
    bool bPropositionExists = false;
    bool bCharMatching = false;
    unsigned int score = 0;

    ifstream infile(filename);
    string line;
    string best;
    string tmpStr;

    if(!infile)
    {
        cout << "Cannot open input file.\n";
        return;
    }

    while ( infile >> line )
    {
        unsigned int tmp = 0;
        tmpStr = tirage;

        if(proposition == line)
        {
            tmp = proposition.length();
            bPropositionExists = true;
        }
        //On skip si le mot est plus court que le meilleur que l'on a actuellement
        else if(line.length() < score)
        {
            continue;
        }
        else
        {
            //On parcourt le mot lettre par lettre pour comparer si elles appartiennent bien au tirage
            for(unsigned int i = 0; i < line.length(); i++)
            {
                bCharMatching = false;

                for(unsigned int j = 0; j < tmpStr.length(); j++)
                {
                    if(tmpStr.at(j) == line.at(i))
                    {
                        tmpStr.at(j) = '_';
                        tmp++;
                        bCharMatching = true;
                        break;
                    }
                }
                //Si la lettre n'appartient pas au tirage, on passe au mot suivant
                if(not bCharMatching)
                {
                    tmp = 0;
                    break;
                }
            }
        }

        //Comparaison de score
        if(tmp > score)
        {
            score = tmp;
            best = line;
        }
    }

    //Le meilleur score est égal au notre: on garde le notre
    if(score == proposition.length() and bPropositionExists)
    {
        best = proposition;
        cout << "Bravo, vous avez trouvé un mot parmi les meilleures solutions possibles !" << endl;
        cout << "C'EST GAGNE  AVEC " << CBOLD << proposition << CDEFAULT << "!!!" << endl;
    }
    else
    {
        //Notre mot existe mais il y a un meilleur score
        if(bPropositionExists)
        {
            cout << CBOLD << proposition << CDEFAULT << " existe mais n'est pas parmi les meilleures solutions" << endl;
            cout << "Avec " << score << " lettres, " << CBOLD << best << CDEFAULT << " fait partie des meilleurs solutions !" << endl;
        }
        //Notre mot n'existe pas
        else
        {
            cout << CRED << "Votre proposition n'existe pas dans notre dictionnaire, désolé !" << CDEFAULT << endl;
            cout << "Avec " << score << " lettres, " << CBOLD << best << CDEFAULT << " fait partie des meilleurs solutions !" << endl;
        }
    }
}

//-------------------------------------------------------------------------

void fnNouveauTour(const char* filename)
{
    int cmptVoyelle;
    bool isVoyelle = false;

    string line;
    string lettresTirees = "_________"; //9 lettres pour le tirage
    ifstream f(filename);

    cout << "C'est parti pour un nouveau mot !" << endl;
    cout << "fichier: " << filename << endl;
    cout << "Choix des lettres: voyelle ou consonne ? (c/v)" << endl;

    //On effectue un tirage random en fonction de si on demande une consonne ou une voyelle
    for(int i = 0; i < 9; i++)
    {
        char c;
        c = getchar();
        fnFlush();

        printf("caractère reçu: %c (%d)", c, c);

        switch(c)
        {
            case 'c':
                isVoyelle = false;
                break;

            case 'v':
                cmptVoyelle ++;
                isVoyelle = true;
                break;

            default:
                //Annulation de la demande -> i--
                i--;
                fnFlush();
                continue;
                break;
        }
        lettresTirees[i] = tolower( fnTirage(isVoyelle) );

        for(int j = 0; j < 9; j++)
            cout << lettresTirees[j] << " ";

        cout << endl;
    }

    //Il faut un minimum de 2 voyelles -> règle
    if(cmptVoyelle < 2){
        cout << CRED << "Tirage nul: 2 voyelles minimum requises !" << CDEFAULT << endl;
        return;
    }

    //On demande à l'utilisateur de faire une proposition avec ce tirage
    string proposition = fnPropositionUsr(lettresTirees);
    cout << endl;

    //Recherche de résultat et comparaison avec notre solution
    fnRechercheDico(filename, proposition, lettresTirees);
}

//-------------------------------------------------------------------------

int main() {

    //Nettoyage du prompt
    system("clear");

    cout << "!!! Bienvenue à \"Des chiffres et des lettres, mais que les lettres\" !!!" << endl; // prints !!!Hello World!!!
	cout << endl;

	bool bReturn = false;
	string str = "../dico_fr.txt";
	const char *filename = str.c_str();

	fnNouveauTour(filename);

	while(not bReturn)
	{

	    //Fin d'un tour -> continue ?
	    char cReturn;

	    cout << endl << "Souhaitez-vous recommencer ? (y/n):" << endl;

	    cReturn = getchar();
	    fnFlush();


	    switch(cReturn)
	    {

	        case 'y':
	        case 'Y':
	            system("clear");
	            fnNouveauTour(filename);
	            break;

	        case 'n':
	        case 'N':
	            bReturn = true;
	            break;

	        default :
	            system("clear");
	            cout    << endl
	                    << "Désolé, je n'ai pas compris: veuillez utiliser '"
	                    << CRED << "y" << CDEFAULT
	                    << "' ou '"
	                    << CRED << "n" << CDEFAULT
	                    << "' svp !" << endl;
	            break;
	    }
	}

	cout << endl;
	cout << "Encore merci d'avoir joué avec nous !" << endl;

	return 0;
}
