#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "util.h"

int client_socket = -1;
char username[MAX_NAME_LEN + 1];
char inbuf[BUFLEN + 1];
char outbuf[MAX_MSG_LEN + 1];

int handle_stdin() {
    int c;
    int count = 0;
    bool overflow = false;
    while((c = fgetc(stdin))) {
        outbuf[count] = c;
        count++;
        if(count == MAX_MSG_LEN) {
            overflow = true;
            fprintf(stderr, "Sorry, limit your message to 1 line of at most %d characters.\n",
                    MAX_MSG_LEN);
            while ((c = fgetc(stdin)) != EOF && c != '\n') {
            }
        }
        if(c == '\0' || c == '\n') {
            break;
        }
    }

    if(outbuf[count-1] == '\n') {
        outbuf[count-1] = '\0';
    }
    
    if(overflow == false) {
        if ((send(client_socket, outbuf, strlen(outbuf) + 1, 0)) == -1) {
            fprintf(stderr, "Error: Failed to send message to server. %s.\n",
                    strerror(errno));
            return EXIT_FAILURE;
        }
    }

    if(strcmp("bye\0", outbuf) == 0) {
        fprintf(stdout, "Goodbye.\n");
        return 1;
    }
    
    return 0;
}

int handle_client_socket() {
    int bytes = recv(client_socket, inbuf, MAX_MSG_LEN + 1, 0);
    if((bytes == -1) && errno != EINTR) {
    fprintf(stderr, "Warning: Failed to receive. %s.\n",
            strerror(errno));
    }

    if(bytes == 0) {
        fprintf(stderr, "\nConnection to server has been lost.\n");
        return EXIT_FAILURE;
    }

    if(bytes == 3) {
        if(strncmp("bye", inbuf, 3)) {
            fprintf(stdout, "\nServer initiated shutdown.\n");
            return 1;
        }
    }

    fprintf(stdout, "%s", inbuf);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server IP> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int serverIP;
    if (!inet_pton(AF_INET, argv[1], &serverIP)) {
        fprintf(stderr, "Error: Invalid input '' received for server IP.\n");
        return EXIT_FAILURE;
    }

    int port;
    if (!parse_int(argv[2], &port, "port")) {
        return EXIT_FAILURE;
    }
    if (port < 1024 || port > 65535) {
        fprintf(stderr, "Error: port must be in range [1024, 65535].\n");
        return EXIT_FAILURE;
    }

    fprintf(stdout, "Enter your username: ");
    while((fgets(inbuf, sizeof(inbuf), stdin)) == NULL || strlen(inbuf) > 20) {
        if(strlen(inbuf) > 20) {
            fprintf(stdout, "Sorry, limit your username to %d characters.\n", MAX_NAME_LEN);
        }
        fprintf(stdout, "Enter your username: ");
    }

    strcpy(username, inbuf);
    username[strlen(username)-1] = '\0';
    fprintf(stdout, "Hello, %s. Let's try to connect to the server.\n", username);

    // Create a reliable, stream socket using TCP.
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Error: Failed to create socket. %s.\n",
                strerror(errno));
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    memset(&server_addr, 0, addrlen);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // Establish the connection to the server.
    if (connect(client_socket, (struct sockaddr *)&server_addr, addrlen) < 0) {
        fprintf(stderr, "Error: Failed to connect to server. %s.\n",
                strerror(errno));
        return EXIT_FAILURE;
    }
    int bytes;
    if ((bytes = recv(client_socket, inbuf, BUFLEN - 1, 0)) < 0) {
        fprintf(stderr, "Error: Failed to receive message from server. %s.\n",
                strerror(errno));
        return EXIT_FAILURE;
    }
    inbuf[bytes] = '\0';
    fprintf(stdout, "\n%s\n\n", inbuf);
    username[strlen(username)] = '\0';
    if ((send(client_socket, username, strlen(username)+1, 0)) == -1) {
        fprintf(stderr, "Error: Failed to send message to server. %s.\n",
                strerror(errno));
        return EXIT_FAILURE;
    }

    fd_set sockset;
    int running = 0;
    
    while(running == 0) {
        if(isatty(STDIN_FILENO)) {
            fprintf(stdout, "[%s]: ", username);
            fflush(stdout);
        }
        FD_ZERO(&sockset);
        FD_SET(client_socket, &sockset);
        FD_SET(STDIN_FILENO, &sockset);

        if (select(client_socket + 1, &sockset, NULL, NULL, NULL) < 0
                && errno != EINTR) {
            fprintf(stderr, "Error: select() failed. %s.\n", strerror(errno));
            return EXIT_FAILURE;
        }

        if (FD_ISSET(client_socket, &sockset)) {
            running = handle_client_socket();
        }

        if (FD_ISSET(STDIN_FILENO, &sockset)) {
            running = handle_stdin();
        }
        
    }
    close(client_socket);
}