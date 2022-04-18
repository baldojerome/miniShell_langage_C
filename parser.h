#ifndef PARSER_H
#define PARSER_H
#include "wordList.h"
#include <assert.h>

// La gestion des quotes est complexe car il faut les eliminer de la chaine
// finale, il faut donc produire une autre chaine sans les quotes
// Exemple : la commande 'l''s' doit donner ls
//

// Le caractère = ne peux pas être traité par le parser quand on le trouve car
// cela dépend du caractère qui suit. Il faut le traiter dans un deuxième passage

// Analyse et découpe la prochaine commande de la ligne
// Basé sur un automate
// La gestion du démarrage d'un mot ou de l'ajout au mot courant
// est géré par WL_add ce qui simplifie ici l'automate

char* parseCmd(char* line,WordList* cmd) {
  bool inQuote1 = false; // Le caractère courant fait partie d'une chaine à simple quote ''
  bool inQuote2 = false; // Le caractère courant fait partie d'une chaien à double quote ""
  // Vide la structure
  WL_reset(cmd);
  // Une commande simple se termine à la fin de la chaine ou sur un des caractères de séparation
  while (*line != 0 && *line != ';' && *line != '&' && *line != '|' && *line != '\n') {
    switch(*line) {

      // Cas des espaces
      case ' ':
      case '\t':
      if (inQuote1 || inQuote2) {
        // on est dans un quote, les espaces perdent leur rôle
          WL_add(cmd,*line);
      } else {
        // Si on est dans un mot, on le termine
        WL_end(cmd);
      }
      break;

      // Cas des simples quotes
      case '\'':
      if (inQuote1) {
        // C'est le quote de fin
        inQuote1 = false;
      } else if (inQuote2){
        // On dans dans des doubles quote : 'est une caractère comme un autre, on l'ajoute au mot
        WL_add(cmd,*line);
      } else {
        // Le simple quote pert sont caractère spéciale dans une chaine a double quote
        inQuote1=true;
      }
      break;

      // Cas des doubles quotes
      case '"':
      if (inQuote2) {
        // C'est le quote de fin
        inQuote2 = false;
      } else if (inQuote1){
        // On dans dans des simple quite : 'est une caractère comme un autre, on l'ajoute au mot
        WL_add(cmd,*line);
      }else {
        // Le double quote pert sont caractère spéciale dans une chaine a simple quote
        inQuote2=true;
      }
      break;

      // Tous les autres caractères sont normaux
      default:
      WL_add(cmd,*line);
    }
    ++line; // Caractère suivant
  }
  // Termine s'il y avait un mot en cours
  WL_end(cmd);
  // Les quotes doivent être fermés
  if (inQuote1) {
    printf("Error: simple quote missing\n");
  }
  if (inQuote2) {
    printf("Error: double quote missing\n");
  }
  return line; // Retourne la position du caractère qui a terminé l'analyse
}

// Automate qui découpe un mot sur le premier caractère '='
// et rend vrai s'il l'a trouvé, les deux mots sont alors dans la structure
// sinon, la structure est incohérente
// Ce caractère ne doit pas être en début de ligne
// Dans le shell, seul le premier caractère = est plus en compte, les autres
// occurences sont des caractères normaux
// ATTENTION : le bash fonctionne différement, une succession d'espaces dans la chaine
// affectée à la valeur est reduite à un seul espace
bool splitOnEqual(char* line,WordList* word) {
  // Vide la structure
  WL_reset(word);
  // Test si en début de ligne
  if (*line == '=') {return false;}
  bool equalFound = false;
  while (*line != 0) {
    if (*line == '=') {
      if (equalFound) {
        // On a déjà trouvé le =, on ne tient pas compte des autres occurences
        WL_add(word,*line);
      } else {
        // C'est la fin du premier mot
        WL_end(word);
        equalFound = true;
      }
    } else {
      // Ajoute au mot courant
      WL_add(word,*line);
    }
    ++line; // Passe au caractère suivant
  }
  // Termine le mot en cours
  WL_end(word);
  return equalFound;
}


#endif
