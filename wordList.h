#ifndef WORDLIST_H
#define WORDLIST_H
#include <assert.h>
// Fonctions pour faciliter la construction d'une série de mots
// caractère par caractère

// Constantes
// const int nbWordMax = 128; // Nombre max d'arguments passé à une commande externe
// const unsigned lineSize = 1024; // Taille max de le ligne en entrée
// ATTENTION: l'écriture ci-dessus n'est pas du C standard
// Utilisation de define à la place
#define nbWordMax 128  // Nombre max d'arguments passé à une commande externe
#define lineSize 1024  // Taille max de le ligne en entrée

// Une liste de mot. Le nombre de mot est dans la structure
// mais la structure se termine avec un pointeur null
// Cette structure est utilisée pour la gestion des arguments d'une commande
typedef struct {
  char data[lineSize]; // Allocation statique de tous les caractères des mots
  char* word[nbWordMax]; // Pointeur sur chaque mot
  int nbWord; // Nombre de mots dans la liste
  int i; // Nombre de caractère du mot nbWord
  // Le signe strictement positif de i permet de savoir
  // si un mot est en cours de construction, sinon, le signe est inversé
  // Cela fait l'économie d'un booléen
} WordList;


// Vide la structure
void WL_reset(WordList* this){
  this->nbWord = 0;
  this->i = 0;
  // Place la marque de fin partout par sécurité
  for (int i = 0; i < nbWordMax; ++i ) {
    this->word[i] = 0;
  }
}
// Constructeur
WordList* WL_new() {
  // Création de l'instance
  WordList* this = malloc(sizeof(WL_new));
  // Initialisation
  WL_reset(this);
  return this;
}

// Destructeur
void WL_del(WordList* this) {
  free(this);
}

// Ajoute un caractère au mot courant ou démarre un nouveau mot
// Donner un zéro à c pour terminer le mot
void WL_add(WordList* this,char c) {
  if (this->i <= 0) {
    // C'est le premier caractère d'un nouveau mot
    this->i = -this->i; // Change d'état, i est la position ou placer ce premier caractère
    this->data[this->i] = c;
    // On note le début du mot
    this->word[this->nbWord] = &this->data[this->i];
    this->i++; // prés pour le caractère suivant
    // Si le caractère et 0 le mot est déjà terminé et est vide
    if (c == 0) { this->i = -this->i;}
  } else {
    if (c == 0) {
      // C'est la fin du mot
      this->data[this->i++] = 0;
      this->nbWord++;
      // Indique que le mot est terminé avec le signe
      this->i = -this->i;
    } else {
      // La suite du mot
      this->data[this->i++] = c;
    }
  }
}

// Termine le mot courant s'il y a un mot en cours de construction
// Sinon ne fait rien
void WL_end(WordList* this) {
  if(this->i > 0) {
    WL_add(this,0);
  }
}

// Retourne le nombre de mots de la structure
int WL_size(WordList* this) {
  // On ne doit pas être en train de créer un mot
  assert(this->i <= 0);
  return this->nbWord;
}

// Recupère un mot particulier
char* WL_get(WordList* this, int i) {
  // On ne doit pas être en train de créer un mot
  assert(this->i <= 0);
  // L'indice doit être correct
  assert(i < this->nbWord);
  return this->word[i];
}

// Affiche la liste des mots
void WL_print(WordList* this) {
  // On ne doit pas être en train de créer un mot
  assert(this->i <= 0);
  int i = 0;
  while (this->word[i] != 0) {
    printf("%d '%s'\n",i,this->word[i]);
    ++i;
  }
}

#endif
