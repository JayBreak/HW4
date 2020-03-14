/////////////////////////////////////////////////////////////////////////////
//
//      Program 4 - otp_decc - Operating System
//      Created By Jakob Eslinger
//      eslingja@oregonstate.edu
//
//      NOTE: This code utilizes provided code from server.c
//
////////////////////////////////////////////////////////////////////////////

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
	char buffer[256];

	if (argc < 4) { fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

        /* Get File and open it */
        FILE* file_stream = fopen(argv[1], "r");

        /* Get Length of the plaintext file */
        fseek(file_stream, 0, SEEK_END);                //Set cursor to EOF
        int plaintext_length = ftell(file_stream);      //Length equals cussor postions

        /* Create variable to hold plaintext */
        char plaintext[plaintext_length];
        memset(plaintext, '\0', plaintext_length);

        /* Save plaintext */
        fseek(file_stream, 0, SEEK_SET);                //Set cursor to BOF
        fgets(plaintext, plaintext_length, file_stream);//Read in the file into plaintext

        /* Close plaintext file */
        fclose(file_stream);

        /* Open Key File */
        file_stream = fopen(argv[2], "r");

        /* Get Length of the key file */
        fseek(file_stream, 0, SEEK_END);                //Set cursor to EOF
        int key_length = ftell(file_stream);            //Length equals cussor postions

        /* Create variable to hold key */
        char key[key_length];
        memset(key, '\0', key_length);

        /* Save key */
        fseek(file_stream, 0, SEEK_SET);                //Set cursor to BOF
        fgets(key, key_length, file_stream);            //Read in the file into plaintext

        /* Close key file */
        fclose(file_stream);

        /* Test of Key and Plaintext are compataible lengths */
        if(key_length < plaintext_length)
        {
                perror("ERROR: key is too short");
                return 1;
        }


        /* Check if plaintext and key contian bad chracters */
        int i;
        for(i = 0; i < plaintext_length-1; i++)           //check plaintext for bad characters
        {
                if(((int)plaintext[i] < 65 || (int)plaintext[i] > 90) && (int)plaintext[i] != 32) //Check if character is outside of ASCII A-Z and is not a space
                {
                        perror("ERROR: plaintext input contains bad characters");
                        return 1;
                }
        }
        for(i = 0; i < plaintext_length-1; i++)           //check key for bad characters
        {
                if(((int)plaintext[i] < 65 || (int)plaintext[i] > 90) && (int)plaintext[i] != 32) //Check if key is character of ASCII A-Z and is not a space
                {
                        perror("ERROR: key input contains bad characters");
                        return 1;
                }
        }
        //set up a verification message and the length of the incoming plaintext
	memset(buffer, '\0', 256);
	sprintf(buffer, "DECRYPTION%d", plaintext_length);

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	// Send verification message to server along with length of plaintext
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	//printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

        /* CHECK VERIFICATION  */
	if(strncmp(buffer, "WRONG_SERVER", 10) == 0)
	{
		perror("CLIENT: Attempted to access the wrong server. Please check port!");
		return 2;
	}

        // Send plaintext to server
	charsWritten = send(socketFD, plaintext, plaintext_length, 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < plaintext_length) printf("CLIENT: WARNING: Not all data written to socket!\n");

        // Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	//printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

        // Send key to server
	charsWritten = send(socketFD, key, key_length, 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < key_length) printf("CLIENT: WARNING: Not all data written to socket!\n");

        // Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	//printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

        // Get encrypted cipher message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");

        /* Print encrypted cipher */
        printf("%s\n", buffer);



	close(socketFD); // Close the socket
	return 0;
}
