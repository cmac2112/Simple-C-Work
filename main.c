#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

int main(int argc, char const* argv[])
{
    int server_fd; //file descriptors for the server and client sockets
    int new_socket;
    ssize_t valread; //stores the number of bytes read from the client
    struct sockaddr_in address; //defines the server address including its IP and port
    int opt = 1; //used for socket options
    socklen_t addrlen = sizeof(address); //holds the size of the address structure
    char buffer[64] = { 0 }; //used to store incoming data from the client
    char* hello = "hello world from the server\n"; //message sent to the client


    //create the socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //attach socket to port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET; //specify the address family
    address.sin_addr.s_addr = INADDR_ANY; //bind to all available interfaces
    address.sin_port = htons(PORT); //convert port number to network byte order

    if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr*) &address, &addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

/*
 *
    int messages_received = 0;
    for (int i = 0; i < 5; i++)
    {
        valread = read(new_socket, buffer, 1024 - 1);
        printf("%s\n", buffer);
        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n + %d\n", i);
        messages_received++;

    }
    */
    int messages_received = 0;
    while (messages_received < 5)
    {
        valread = read(new_socket, buffer, sizeof(buffer) - 1);
        if (valread == sizeof(buffer) - 1) {
            fprintf(stderr, "Received message too large for buffer\n");
            send(new_socket, "Error: message too large\n", 24, 0);
            return 1;
        }
        if (valread > 0) {
            buffer[valread] = '\0';
            printf("%s\n", buffer);
            send(new_socket, hello, strlen(hello), 0);
            printf("Hello message sent %d\n", messages_received + 1);
            messages_received++;
        }
    }

    close(new_socket);

    close(server_fd);
    return 0;

}