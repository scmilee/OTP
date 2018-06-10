#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>


//global alphabet
char* alphabet[27] = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"," "};
int bgProcesses[100];
int bgCount = 0;

//my checkbg function from smallsh
void checkBg() {
  int currentStat;
  for(int i = 0; i < bgCount; i++) 
  { 
    waitpid(bgProcesses[i], &currentStat, WNOHANG);
  }
}

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

//gets the index of a character in the alphabet. IE: L == 11
int index_of(char search) {

    for (int i = 0; i < 27; ++i)
    {
      if (*alphabet[i]== search)
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
  //printf("%d, %d, %d\n", trueindex,index,traverseCount );
  return trueindex;
}
//got this file read function from
// https://stackoverflow.com/questions/3463426/in-c-how-should-i-read-a-text-file-and-print-all-strings
char* ReadFile(char *filename, int* string_size){
   char *buffer = NULL;
   int  read_size;
   FILE *handler = fopen(filename, "r");
//if theres a file
   if (handler)
   {
      fseek(handler, 0, SEEK_END);
      //get the size
      *string_size = ftell(handler);
      // go back to the start of the file
      rewind(handler);
      buffer = (char*) malloc(sizeof(char) * (*string_size + 1) );
      //read it into the mallocd buffer
      read_size = fread(buffer, sizeof(char), *string_size, handler);
      buffer[*string_size] = '\0';

      //clear and free buffer when done
      if (*string_size != read_size)
      {
        free(buffer);
        buffer = NULL;
      }
      //close the file
      fclose(handler);
  }

return buffer;
}

void encryptFile(char* keyfile, char* textfile, int textfileSize,char *buffer,int establishedConnectionFD){
  
  int traverseCount;
  int index;
  int encryptedIndex;
  int charsRead;

  for (int i = 0; i < textfileSize - 1; ++i){
    //get the index of both letters from each file
    traverseCount = index_of(keyfile[i]);
    index = index_of(textfile[i]);

    if (index == -1)
    {
      //send an error back to the client for complete closure
      charsRead = send(establishedConnectionFD, "", 0, 0); 
      error("Error invalid characters in file.\n");
      close(establishedConnectionFD); // Close the existing socket which is connected to the client
      exit(1);
    }
    traverseCount += index;
    //++ to account for it being an array index
    traverseCount++;
    //get the destined index from the looper
    encryptedIndex = looper(index, traverseCount);
    //printf("%c + %c = %d\n" ,textfile[i],keyfile[i], encryptedIndex );
  	if(i == 0){
  	 strcpy(buffer, alphabet[encryptedIndex]);
  	}
  	else{
      	strcat(buffer, alphabet[encryptedIndex]);
  	}    //reset for next iteration
      index = 0;
      traverseCount = 0;
      encryptedIndex = 0;

  }

} 

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[70001];
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

  pid_t pid;
  //enter into a loop for main, so it will constantly accept new connections
  while(1){
  	// Accept a connection, blocking if one is not available until one connects
  	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
  	establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
  	if (establishedConnectionFD < 0) error("ERROR on accept");
    //fork the process if the connection has been established
    pid = fork();
    int currentStatus;
    //catch fork failure
    if(pid < 0){
      error("Error forking child");
      exit(1);
        }
    if (pid == 0){
    	// Get the message from the client and display it
    	memset(buffer, '\0', 70001);
    	charsRead = recv(establishedConnectionFD, buffer, 70000, 0); // Read the client's message from the socket
    	if (charsRead < 0) error("ERROR reading from socket");
    //	printf("SERVER: I received this from the client: \"%s\"\n", buffer);
      
      char* keyAndtext;
      strtok_r(buffer, "\n", &keyAndtext);
      //the buffer now holds the client ID for checking

      if (strcmp(buffer, "otp_enc")!= 0)
      {
        //send an error back to the client for complete closure
         charsRead = send(establishedConnectionFD, "ERROR only otp_enc can talk with this server.;", 46, 0); 
        close(establishedConnectionFD); // Close the existing socket which is connected to the client
         exit(0);
      }
      //key will hold the key while buffer will hold the textfile for encryption 
      char* key;
      strtok_r(keyAndtext, "\n", &key );
      //create and fill buffers for the key and file contents
      int keyfileSize;
      int textfileSize;

      char* keyfile = ReadFile(key,&keyfileSize);
      char* textfile = ReadFile(keyAndtext,&textfileSize);
  
      //throw error if the keyfile size is too small
      if (keyfileSize < textfileSize){
        //send an error back to the client for complete closure
         charsRead = send(establishedConnectionFD, "ERROR Key Size is Too Small;", 28, 0); 
        close(establishedConnectionFD); // Close the existing socket which is connected to the client
         exit(1);
      }

      char * encryptedFile;
      encryptedFile = malloc(textfileSize + 1);
      encryptFile(keyfile, textfile, textfileSize, encryptedFile, establishedConnectionFD);
      //printf("here it is encoded :%s\n", encryptedFile);
      strcat(encryptedFile, ";");
    	// Send a Success message back to the client
     
    	 charsRead = send(establishedConnectionFD, encryptedFile, textfileSize + 2, 0); // Send success back
      free(encryptedFile);
    	if (charsRead < 0) error("ERROR writing to socket");
    	close(establishedConnectionFD); // Close the existing socket which is connected to the client
      //exit for the child
      _exit(0);
    }

    bgProcesses[bgCount] = pid;
    bgCount++;
    waitpid(pid, &currentStatus, WNOHANG);
    checkBg();
  }
	close(listenSocketFD); // Close the listening socket
	return 0; 
}
