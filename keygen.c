/////////////////////////////////////////////////////////////////////////////
//
//      Program 4 - keygen - Operating System
//      Created By Jakob Eslinger
//      eslingja@oregonstate.edu
//
//      Code outputs a random key that is generated with a length of n,
//      being the legth that is an input. Output is sent to stdout. Key
//      contains a newline at the end.
//
////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ASCII_OFFSET 65

/******************************* Main Function *******************************/
int main(int argc, char *argv[])
{
        /* Check if there are enough inputs */
        if(argc < 2)
        {
                perror("Error: Too Few Arguments");
                return 1;
        }

        /* Setup seed for random fucntion */
        srand(time(NULL));

        /* Get length */
        int length = atoi(argv[1]);                  //Convert first argument from string to int

        /* Generate Key */
        char* key = malloc(sizeof(char) * (length + 1));        //allocate space for key +1 for endline
        int i;
        for(i = 0; i < length; i++)
        {
                int randomChar = rand() % 27;           //27 diffetned options for characters
                randomChar += ASCII_OFFSET;             //Offset the option to equeal a letter in ASCII

                if(randomChar == 91)                    //If randomChar is 91, then replace to with a spcae
                {
                        randomChar = 32;                //Space is 32 in ASCII
                }

                key[i] = (char) randomChar;             //Typecast output to a char and save it in the key array
        }
        key[i] = '\n';                              //add newline character

        /* Output Key*/
        printf("%s", key);

        /* Free Memory */
        free(key);

        return 0;
}
