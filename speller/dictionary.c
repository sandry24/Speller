// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

const unsigned int N = 31337;
const unsigned int P = 31;

// helper array
int p_pow[LENGTH];

// Hash table
node *table[N];

FILE *inptr = NULL;
int table_size = 0;

// Helper function to precompute powers for hashing
void compute_pow(){
    p_pow[0] = 1;
    for (int i = 1; i < LENGTH; i++){
        p_pow[i] = p_pow[i-1] * P;
        p_pow[i] %= N;
    }
}

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    char word_lowercase[LENGTH + 1];
    for (int i = 0; i <= strlen(word); i++)
        word_lowercase[i] = tolower(word[i]);

    int hashed_word = hash(word_lowercase);
    node *temp = table[hashed_word];
    while (temp != NULL){
        if (strcmp(temp->word, word_lowercase) == 0)
            return true;
        temp = temp->next;
    }
    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    unsigned int hash_value = 0;
    for (int i = 0; i < strlen(word); i++){
        hash_value += (toupper(word[i]) - 'A' + 1) * p_pow[i];
        hash_value %= N;
    }
    return hash_value;
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    inptr = fopen(dictionary, "r");
    if (inptr == NULL)
        return false;

    compute_pow(); // precomputation for hashing
    for (int i = 0; i < N; i++){
        table[i] = NULL;
    }

    char line[LENGTH+1];
    while (fgets(line, sizeof(line), inptr)) {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n')
            line[--len] = '\0';

        if (len <= 0)
            continue;

        // make new node
        int hash_value = hash(line);
        node *new_word = malloc(sizeof(node));
        if (new_word == NULL){
            fclose(inptr);
            return false;
        }

        // add node to the front of the queue
        strcpy(new_word->word, line);
        new_word->next = table[hash_value];
        table[hash_value] = new_word;
        table_size++; // increment table_size for efficient size()
    }

    fclose(inptr);
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    if (inptr == NULL)
        return 0;
    return table_size;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    if (inptr == NULL)
        return false;

    for (int i = 0; i < N; i++){
        node *temp = table[i];
        while (temp != NULL){
            node *next = temp->next;
            free(temp);
            temp = next;
        }
    }

    return true;
}
