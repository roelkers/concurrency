#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h>
#include<string.h> 
#include<sys/socket.h> 
#include<sys/types.h> 
#include<netdb.h> 
#include<netinet/in.h> 
#include<arpa/inet.h>
#include <pthread.h>

#define MESSAGE_LENGTH 500000 // Maximum number of data that can transfer
#define PORT 8000 // port number to connect
#define SA struct sockaddr // Creating Macro for the socketaddr as SA
#define NUMTHREADS 1;

int thread_counter;

void getFileName(char * message, char substr[80]) {
    int i = 4; //GET+1
    while(message[i] != '\0' && message[i] != EOF && message[i] != ' ') {
        i++;
    }
    char * start = &message[4];
    char * end = &message[i];
    memcpy(substr, start, end-start);
}

void *handleHTTPRequest(void* ptr) {
    char message[MESSAGE_LENGTH];
    char clientMessage[MESSAGE_LENGTH];
    FILE * requestedFile;
    char fileName[80];
    char fileContent[450000];
    int messageOffset = 0;

    /* close(sockert_file_descriptor); */

    int * socket_connection = (int *) ptr;
    int connection = (int) *socket_connection;

    bzero(clientMessage, MESSAGE_LENGTH);
    bzero(message, MESSAGE_LENGTH);

    ssize_t readbytes = read(connection, clientMessage, sizeof(clientMessage));
    clientMessage[readbytes] = '\0';

    messageOffset += snprintf(message, MESSAGE_LENGTH, "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\n");

    char * fromGet = strstr(clientMessage, "GET");
    if(fromGet) {
        bzero(fileContent, 512);
        bzero(fileName, 80);
        char character;

        getFileName(fromGet, fileName);
        size_t fileSize = 0;
        size_t openFileOffset = 0;

        //attempt to open file
        requestedFile = fopen(fileName, "rt");
        if(requestedFile == 0) {
          messageOffset += snprintf(message+messageOffset, MESSAGE_LENGTH, "Content-Type: text/html\r\n");
          messageOffset += snprintf(message+messageOffset, MESSAGE_LENGTH, "\r\n\r\n<html><div>Your requested file is: %s. The file was not found</div>", fileName);
          messageOffset += sprintf(message+messageOffset, "</html>");
          perror("fopen");
        } else {
          messageOffset += snprintf(message+messageOffset, MESSAGE_LENGTH, "Content-Type: text/html\r\n");

          while(!feof(requestedFile)) {
            character = fgetc(requestedFile);
            if(character != EOF) {
                openFileOffset += snprintf(fileContent+openFileOffset, 512,  "%c", character);
                fileSize ++;
            }
          }
          messageOffset += snprintf(message+messageOffset, MESSAGE_LENGTH, "Content-Length: %lu\r\n\r\n", fileSize);
          messageOffset += snprintf(message+messageOffset, MESSAGE_LENGTH, "%s", fileContent);

          fclose(requestedFile);
        }
    }

     // Sending the data to the server by storing the number of bytes transferred in bytes variable 
    ssize_t bytes = write(connection, message, MESSAGE_LENGTH);
    
    // If the number of bytes is >= 0 then the data is sent successfully
    if(bytes >= 0){
        /* printf("Data successfully sent to the client.!\n"); */
    }
    
    // Closing the Socket Connection
    close(connection);
    /* thread_counter--; */
    pthread_exit(NULL);
}

int main(){
    struct sockaddr_in serveraddress, client;
    socklen_t length;
    int sockert_file_descriptor, socket_connection, bind_status, connection_status;
    pthread_t thread;

    // Creating the Socket
    sockert_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if(sockert_file_descriptor == -1){
        printf("Scoket creation failed.!\n");
        exit(1);
    }

    // Erases the memory
    bzero(&serveraddress, sizeof(serveraddress));

    // Server Properties
    serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // Setting the port number
    serveraddress.sin_port = htons(PORT);
    // Protocol family
    serveraddress.sin_family = AF_INET;

    // Binding the newly created socket with the given Ip Address
    bind_status = bind(sockert_file_descriptor, (SA*)&serveraddress, sizeof(serveraddress));

    if(bind_status == -1){
        printf("Socket binding failed.!\n");
        exit(1);
    }

    // Server is listening for new creation
    connection_status = listen(sockert_file_descriptor, 5);

    if(connection_status == -1){
        printf("Socket is unable to listen for new connections.!\n");
        exit(1);
    } else {
        printf("Server is listening for new connection: \n");
    }

    length =  sizeof(client);

    // Communication Establishment
    while(1){
        
        socket_connection = accept(sockert_file_descriptor, (SA*)&client, &length);

        if(socket_connection == -1){
            printf("Server is unable to accept the data from client.!\n");
            exit(1);
        }
        pthread_create(&thread, NULL, handleHTTPRequest, &socket_connection);
        /* thread_counter++; */
        /* printf("Currently %i threads are running.\n", thread_counter); */
    }

    return 0;
}
