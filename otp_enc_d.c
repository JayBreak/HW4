/////////////////////////////////////////////////////////////////////////////
//
//      Program 4 - otp_enc_d - Operating System
//      Created By Jakob Eslinger
//      eslingja@oregonstate.edu
//
//      NOTE: This code utilizes provided code from server.c
//
////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define ASCII_OFFSET 64

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues


char* encrypt(char * cipher, char* key, int length)
{
        int i;
        for(i = 0; i < length-1; i++)
        {
                int cipher_index = (int) cipher[i];
                int key_index = (int) key[i];

                /* Offset ASCII of cipher */
                if(cipher_index == 32)          //if 32, then it is a space
                {
                        cipher_index = 0;       //set to 0
                }
                else
                {
                        cipher_index = cipher_index - ASCII_OFFSET;
                }

                /* Offset ASCII of key */
                if(key_index == 32)          //if 32, then it is a space
                {
                        key_index = 0;       //set to 0
                }
                else
                {
                        key_index = key_index - ASCII_OFFSET;
                }

                /* Add key value and mod */
                cipher_index = (cipher_index + key_index) % 27;

                /* Convert back to ASCII and save character */
                if(cipher_index == 0)          //if 32, then it is a space
                {
                        cipher_index = 32;       //set to 0
                }
                else
                {
                        cipher_index = cipher_index + ASCII_OFFSET;
                }
                cipher[i] = (char) cipher_index;
        }
        return cipher;
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

        while(1 == 1)   //loop constantley and listen for connectionss
        {
                // Accept a connection, blocking if one is not available until one connects
        	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
        	establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
        	if (establishedConnectionFD < 0) error("ERROR on accept");

                // Create Fork to encrypt input
                pid_t child_pid = fork();
                switch (child_pid)
                {
                        case -1:
                                perror("Hull Breach!");
                                exit(1);
                                break;
                        case 0:
                                // Get the message from the client and display it
                                memset(buffer, '\0', 256);
                                charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
                                if (charsRead < 0) error("ERROR reading from socket");

                                /* verification check */
                                if(strncmp(buffer, "encryption", 10) == 0)
                                {
                                        // Send a Success message back to the client
                                	charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
                                	if (charsRead < 0) error("ERROR writing to socket");

                                        //get length of message
                                        int length = atoi(&buffer[10]);

                                        // Create cipher variable
                                        char cipher[length-1];
                                        memset(cipher, '\0', length-1);

                                        // Recieve cipher
                                        charsRead = recv(establishedConnectionFD, cipher, length, 0); // Read the client's message from the socket
                                        if (charsRead < 0) error("ERROR reading from socket");

                                        // Send a Success message back to the client
                                	charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
                                	if (charsRead < 0) error("ERROR writing to socket");

                                        // Create key variable
                                        char key[length];
                                        memset(key, '\0', length);

                                        // Recieve key
                                        charsRead = recv(establishedConnectionFD, key, length, 0); // Read the client's message from the socket
                                        if (charsRead < 0) error("ERROR reading from socket");

                                        // Send a Success message back to the client
                                	charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
                                	if (charsRead < 0) error("ERROR writing to socket");

                                        /********** ENCRYPT **********/
                                        encrypt(cipher, key, length);

                                        // Send a encrypted cipher back to the client
                                	charsRead = send(establishedConnectionFD, cipher, length, 0); // Send cipher back
                                	if (charsRead < 0) error("ERROR writing to socket");

                                        close(establishedConnectionFD); // Close the existing socket which is connected to the client
                                        exit(0);                        //exit child
                                }
                                //Else if verification fails
                                else
                                {
                                        //print error
                                        perror("ERROR: trying to connect to otp_enc_d with otp_dec");
                                        // Send a message back to client, informing them they have used the wrong server
                                	charsRead = send(establishedConnectionFD, "WRONG_SERVER", 12, 0); // Send success back
                                	if (charsRead < 0) error("ERROR writing to socket");

                                        close(establishedConnectionFD); // Close the existing socket which is connected to the client
                                        exit(1);                        //exit child
                                }


                                break;
                }
        }

	close(listenSocketFD); // Close the listening socket
	return 0;
}
