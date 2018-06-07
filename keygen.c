#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

//global alphabet
char* alpahbet[27] = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"," "};

void generateKey(char* key, int keySize){
  int random_number;
  //loop through the key array and set each one to a random letter in the alphabet between  0 and 26  
  for (int i = 0; i < keySize; ++i)
  {
    random_number = rand() % 27 + 0;
    key[i] = *alpahbet[random_number];
  }
  //top off the key array with a new line
  key[keySize] = '\n';
}

int main(int argc, char *argv[])
{
  //seed the randomness
  srand(time(NULL));
  //atoi from 
  //https://stackoverflow.com/questions/2237980/how-do-i-convert-a-char-to-an-int
  int keySize = atoi(argv[1]);
  //plus 1 for the extra \n
  char key[keySize + 1]; 

  //if there's more/less than one arguement or if atoi returns 0 IE: 'foobar' returns 0, 0 returns 0 then exit 
  if (argc != 2 || keySize == 0)
   {
    fprintf( stderr, "Incorrect arguements for keygen\n");
     exit(1);
  }
  //generate it
  generateKey(key, keySize);
  char* buffer;
  //remove ugly memory from key
  strtok_r(key, "\n" , &buffer);
  strcat(key, "\n");
  //print it out 
  printf("%s", key);

  //exit dis bish
  return 0;
}