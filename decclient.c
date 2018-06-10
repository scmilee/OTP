#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
  int socketFD, portNumber, charsWritten, charsRead;
  struct sockaddr_in serverAddress;
  struct hostent* serverHostInfo;
  char buffer[70001];
  char buff[70001];
  memset(buffer, '\0', sizeof(buffer));
    
  if (argc != 4) { fprintf(stderr,"USAGE: %s text key port\n", argv[0]); exit(0); } // Check usage & args

  // Set up the server address struct
  memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
  portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
  serverAddress.sin_family = AF_INET; // Create a network-capable socket
  serverAddress.sin_port = htons(portNumber); // Store the port number
  serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
  if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
  memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

  // Set up the socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
  if (socketFD < 0) error("CLIENT: ERROR opening socket");
  
  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
    error("CLIENT: ERROR connecting");

  // Get input message from user
  char * key = argv[2];
  char * text= argv[1];
  char* rackage;
  char* identity = "otp_dec";
  rackage = malloc(strlen(key)+strlen(text) +strlen(identity)+ 2);

  //put the identity into the package
  strcpy(rackage, identity);
  strcat(rackage, "\n");
  //put the text file name into the package
  strcat(rackage, text);
  strcat(rackage, "\n");
  //put the keyfile name into the package
  strcat(rackage, key);
  strcat(rackage, "\0");

  strcpy(buffer,rackage);

  // Send message to server

  charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
  if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
  if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

  // Get return message from server
  memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
  memset(buff, '\0', sizeof(buffer));
  char* removecolon;
  
  while(strchr(buffer, ';') == NULL){
    memset(buffer, '\0', sizeof(buffer));
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
    strcat(buff, buffer);
  }
  
  //remove the identifier for the end of the file
  removecolon = strchr(buff, ';');
  *removecolon = '\0';

  //checking to see if message from server was an error or not
  //to output to perror according to specs
  char* errorToken;
  errorToken = strtok(buffer, " ");

  if (strcmp(errorToken, "ERROR") == 0)
  {
    strcat(errorToken, buffer); 
    error(buff);
  }
  //strcat(errorToken, buff);
  printf("%s\n", buff );
  if (charsRead < 0) error("CLIENT: ERROR reading from socket");
  close(socketFD); // Close the socket
  free(rackage);
  return 0;
}
