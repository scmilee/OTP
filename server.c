#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>


//global alphabet
char* alpahbet[27] = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"," "};


void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

//gets the index of a character in the alphabet. IE: L == 11
int index_of(char* search) {
    for (int i = 0; i < 27; ++i)
    {
      printf("%s\n", search );
      if (strcmp(alpahbet[i], search) == 0)
      {
        return i;
      }
    }
    return -1;
}
//loops through a faux alphabet starting at the index and ending at the traverse count / total IE: L + B = N

int looper(int index, int traverseCount){
  int trueindex = index;
  for (int i = index; i < traverseCount; ++i)
  {
    trueindex++;
    if (trueindex > 26)
    {
      trueindex = 0;
    }
  }
  return trueindex;
}
//got this file read function from
// https://stackoverflow.com/questions/3463426/in-c-how-should-i-read-a-text-file-and-print-all-strings
char* ReadFile(char *filename, int* string_size)
{
   char *buffer = NULL;
   int  read_size;
   FILE *handler = fopen(filename, "r");

   if (handler)
   {
      // Seek the last byte of the file
      fseek(handler, 0, SEEK_END);
      // Offset from the first to the last byte, or in other words, filesize
      *string_size = ftell(handler);
      // go back to the start of the file
      rewind(handler);

      // Allocate a string that can hold it all
      buffer = (char*) malloc(sizeof(char) * (*string_size + 1) );
      // Read it all in one operation
      read_size = fread(buffer, sizeof(char), *string_size, handler);
      // fread doesn't set it so put a \0 in the last position
      // and buffer is now officially a string
      buffer[*string_size] = '\0';

      if (*string_size != read_size)
      {
        // Something went wrong, throw away the memory and set
        // the buffer to NULL
        free(buffer);
        buffer = NULL;
      }
      // Always remember to close the file.
      fclose(handler);
  }

return buffer;
}

char* encryptFile(char* keyfile, char* textfile, int keyfileSize, int textfileSize){
  char *buffer = NULL;
} 

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[256];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	// Accept a connection, blocking if one is not available until one connects
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
	establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
	if (establishedConnectionFD < 0) error("ERROR on accept");

	// Get the message from the client and display it
	memset(buffer, '\0', 256);
	charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
	if (charsRead < 0) error("ERROR reading from socket");
	printf("SERVER: I received this from the client: \"%s\"\n", buffer);
  
  //key will hold the key while buffer will hold the textfile for encryption 
  char* key;
  strtok_r(buffer, "\n", &key);

  //create and fill buffers for the key and file contents
  int keyfileSize;
  int textfileSize;

  char* keyfile = ReadFile(key,&keyfileSize);
  char* textfile = ReadFile(buffer,&textfileSize);

  //keyfile size is two char bigger from /0 and /n
  //throw error if the keyfile size is too small
  if (keyfileSize <= textfileSize + 1) error("ERROR Key is too small");
  
  char* encryptedFile = encryptFile(keyfile, textfile, keyfileSize, textfileSize);


 



	// Send a Success message back to the client
	charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
	if (charsRead < 0) error("ERROR writing to socket");
	close(establishedConnectionFD); // Close the existing socket which is connected to the client
	close(listenSocketFD); // Close the listening socket
	return 0; 
}
