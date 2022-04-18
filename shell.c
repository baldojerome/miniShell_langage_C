#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include "wordList.h"
#include "parser.h"
#include <errno.h>



//SIGACTION
// Type handler de signal
typedef void (*sighandler_t)(int);

// Variables globales pour le handler
// Dernier enfant terminé
int pidret;
// Nb enfants restants
int nbenf;

// Fonction gestionnaire de signal SIGCHLD
void traite_sig(int numsig)
{
	// On attend tout enfant, en mode non bloquant
	// sans récupérer le code de terminaison
	while(1)
	{
		pidret = waitpid(-1, NULL, WNOHANG);
		if(!pidret)
		{
			// Aucun enfant terminé en attente
			// On sort de la boucle
			return;
		}
		else
		{
			if(pidret == -1)
			{
				if(errno == ECHILD)
				{
					//Il n'y a plus de processus enfant, on sort
					return;
				}
				perror("waitpid ");
				_exit(errno);
			}
			else
			{
				nbenf--;
			}
		}
	}	
}



//fonction pour enlever le \n pour le exit(sortie du programme)
void trim(char* s)
{
    int  i = 0;
    while ( s[i] != '\n' && s[i] != 0)
    {
        i++;
    }
    s[i] = 0;
}

//fonction pour savoir s'il y a un & au premier et dernier mot
bool presentECom(WordList* cmd)
{
    char result;
    int x = 0;
    int countLetter = 0;
    int position= 0;
    bool presentEC = false;
    
    int compteur = -(cmd->i); // car i est négatif
            
    while(x < compteur)
    {
        result = cmd->word[0][x];
        //printf("%c\n", result);
        if(result != '&')
        {
            countLetter++;
        }
        else
        {
            position = x;
        }
        x++;
    }
            
    if(countLetter != compteur && position == compteur-2)
    {
        presentEC = true;
    }
    else if(countLetter == compteur)
    {
        presentEC = false;
    }
    else
    {
        presentEC = false;
        //perror("erreur de commande");
        fprintf(stderr, "commande erronee. Le & doit être en fin de mots.\n");
    }
    
    return presentEC;
}


int main()
{
    
    //SIGACTION
    // creation des variables pour le sigaction
    pid_t pidc;
	int coderet;
	int signum;
	struct sigaction act, oldact;
    
    //creation des variables pour créer une ligne de mots
    int  a  = 0;
    char line[lineSize];
    int booleen;
    WordList* cmd = WL_new();

    
    while(a == 0)
    {
        //SIGACTION
        // On "arme" la gestion du signal SIGCHLD
	    act.sa_handler = traite_sig;
	    sigemptyset(&act.sa_mask);
	    // Aucune option particulière
	    act.sa_flags = 0;
	    coderet = sigaction(SIGCHLD, &act, &oldact);
	    if(coderet== -1) //verification du sigaction
	    {
		    perror("sigaction ");
		    return errno;
	    }
        
        WL_reset(cmd);
        bool gestionArriere = false;
        bool affectation = false; //reinitiliser les variables à chaque tour de boucle
        printf("assh$   ");

        if(fgets (line, lineSize, stdin) != NULL)
        {
            
            trim(line); //appel de la fonction pour enleverle \n
            booleen = strcmp (line, "exit"); //comparaison entre la commande et l'exit
            if (booleen == 0)
            {
                printf("Fin du programme assh. A bientot!!\n");
                return 0;
            }
            
            affectation = splitOnEqual(line,cmd);  //test si = dans la ligne          
            if(presentECom(cmd)) //si & dans la ligne urtout à la fin du premier /dernier mot 
            {
                gestionArriere = true;
            } 
            if(!affectation)
            {
                printf("Ligne lue : %s\n", line);
                parseCmd(line,cmd);

                pid_t c_pid = fork();
                if (c_pid == -1) 
                {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }

                if (c_pid == 0) // on est dans l'enfant
                {
                    //printf("printed from child process - %d\n", getpid());
                    int coderet;
			        pid_t monpid;
                                      
                    int status_code = execvp(cmd->word[0], cmd->word);
                    if (status_code == -1) 
                    {
                        perror("commande non trouvé!\n");
                        return 1;
                    }
                    
                    // Code retour = Dernier octet du PID
                    coderet = monpid&0377;
			        _exit(coderet);
                } 
                else //on est dans le père
                {
                    //printf("printed from parent process - %d\n", getpid());
                    nbenf++;
                    if(gestionArriere)
                    {
                        printf("je fais le processus en arrière plan.\n");
                        pidret = waitpid(-1, NULL, WNOHANG);
                        if(pidret ==  SIGCHLD)
                        {
                            wait(NULL);
                        }
                        
	                } 
                    else
                    {
                        wait(NULL);
                    }
                    
                }
            }
            else
            {
                //printf("traitement de la variable.\n");
                //mettre premier argument dans une premier chaine 
                //mettre deuxieme argument dans une chaine 
                //chai 1 = chaine2;

                //difficulte : comment  avoir le word1 qui devient nom de variable pour etre rappeler 
            }
            
        }
    }
    return 0;
}