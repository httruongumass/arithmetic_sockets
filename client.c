#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <math.h>

int main(int argc, char *argv[])
{
    //Argument checking
    if(argc != 4)
    {
        printf("Invalid argument.\n");
        return -1;
    }

    //Arguments
    char *id = argv[1], *ip = argv[3];
    int port = atoi(argv[2]);

    //Initialize varaible to store network info
    struct sockaddr_in server;
    char call[1024], response[1024], flag[1024];

    //Variables for arithmetic to be stored
    int NUM1, NUM2, ans;
    char operator;

    //Generating key for server to identify
    char key[18 + sizeof(argv[1])] = "cs230 HELLO ";
    strcat(key , id);
    strcat(key, "\n");

    //Create socket (IPv4, TCP Protocol)
    int sd = socket(AF_INET, SOCK_STREAM, 0);

    //Socket descriptor check
    if(sd == -1)
    {
        printf("ERROR: Socket creation failure\n");
        return -1;
    }

    //Prepares server info
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    //Connect to server, checks if connection is successful
    if(connect(sd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("ERROR: Could not connect to server...\n");
        return -1;
    }

    //Sending initial identifier key to server
    if(send(sd, key, strlen(key), 0) < 0)
    {
        printf("ERROR: Could not send key...");
        return -1;
    }

    int status = 1;
    while(status == 1)
    {
        //Clears strings
        call[0] = '\0';
        response[0] = '\0';

        //Received problem
        if(recv(sd, response, 1024, 0) < 0)
        {
            printf("ERROR: Could not receive response");
            return -1;
        }
        
        //Checks if server sends flag
        const char *bye = "BYE";
        if(strstr(response, bye) != NULL)
        {
            //If server sends flag, extract flag
            const char *needle = "cs230";
            size_t needle_length = strlen(needle);
            const char *needle_pos = strstr(response, needle);
            sscanf(needle_pos + needle_length, "%s", flag);

            //Breaks while loop
            status = 0;
        }
        else{
            //Looks for STATUS (To find expression to solve)
            const char *needle = "STATUS";
            size_t needle_length = strlen(needle);
            const char *needle_pos = strstr(response, needle);
            char expression[64];
            
            //Extracts expression into respective variables
            sscanf(needle_pos + needle_length, "%d %c %d", &NUM1, &operator, &NUM2);
            
            //Solves expression
            if(operator == '+')
            {
                ans = NUM1 + NUM2;
                printf("%d + %d = %d\n", NUM1, NUM2, ans);
            }
            else if(operator == '-')
            {
                ans = NUM1 - NUM2;
                printf("%d - %d = %d\n", NUM1, NUM2, ans);
            }
            else if(operator == '*')
            {
                ans = NUM1 * NUM2;
                printf("%d * %d = %d\n", NUM1, NUM2, ans);
            }
            else if(operator == '/')
            {
                ans = floor(NUM1 / NUM2);
                printf("%d / %d = %d\n", NUM1, NUM2, ans);
            }
            
            //Stores answer to temp to be translated to char
            char temp[1024];
            sprintf(temp, "%d", ans);

            //Generating call to server
            strcat(call, "cs230 ");
            strcat(call, temp);
            strcat(call, "\n");

            //Outputs call to server
            printf("> %s", call);

            //Sends answer to server
            if(send(sd, call, strlen(call), 0) < 0)
            {
                printf("ERROR: Could not send answer...");
                return -1;
            }
        }
    }
    printf("FLAG: %s\n", flag);
    close(sd);
    return 0;
}