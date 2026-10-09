#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <stdbool.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>

#define MAX_CONNECTIONS 2
#define BUFFER_SIZE     1024
#define DATA_FILE       "/var/tmp/aesdsocketdata"

int server_fd = -1;
int client_fd = -1;
FILE *file = NULL;

void start_daemon()
{
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0)
    {
        exit(EXIT_FAILURE);
    }
}

void handle_signal(int signal)
{
    syslog(LOG_INFO, "Caught signal, exiting");

    if (client_fd >= 0)
    {
        shutdown(client_fd, SHUT_RDWR);
        close(client_fd);
    }

    if (server_fd >= 0)
    {
        shutdown(server_fd, SHUT_RDWR);
        close(server_fd);
    }

    if (file != NULL)
    {
        fclose(file);
        file = NULL;
    }
    if (remove(DATA_FILE) != 0)
    {
        syslog(LOG_ERR, "Error deleting file");
        exit(EXIT_FAILURE);
    }

    closelog();
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    openlog(NULL, 0, LOG_USER);
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    int status;
    int opt;
    struct addrinfo hints, *servinfo;
    struct sockaddr client_addr;
    socklen_t addr_size = sizeof(client_addr);
    unsigned char buffer[BUFFER_SIZE];
    ssize_t bytes_recv;
    char *line = NULL;
    size_t cap;
    ssize_t len;
    bool daemon_flag = ((argc == 2 && strcmp(argv[1], "-d") == 0) ? true : false);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        syslog(LOG_ERR, "Failed to create socket");
        closelog();
        return -1;
    }

    opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((status = getaddrinfo(NULL, "9000", &hints, &servinfo)) != 0)
    {
        syslog(LOG_ERR, "getaddrinfo failed");
        close(server_fd);
        closelog();
        return -1;
    }

    if (bind(server_fd, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
    {
        syslog(LOG_ERR, "Failed to bind socket");
        close(server_fd);
        freeaddrinfo(servinfo);
        closelog();
        return -1;
    }
    freeaddrinfo(servinfo);

    if (listen(server_fd, MAX_CONNECTIONS) < 0)
    {
        close(server_fd);
    }

    file = fopen(DATA_FILE, "ab+");
    if (!file)
    {
        printf("error here");
        syslog(LOG_ERR, "ERROR: Could not open file");
        closelog();
        return 1;
    }

    if (daemon_flag)
    {
        start_daemon();
    }

    while (1)
    {
        client_fd = accept(server_fd, &client_addr, &addr_size);
        if (client_fd < 0)
        {
            break;
        }
        syslog(LOG_INFO, "Accepted connection from %X", ntohl(((struct sockaddr_in *)&client_addr)->sin_addr.s_addr));

        while ((bytes_recv = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0)
        {
            fwrite(buffer, 1, bytes_recv, file);
            fflush(file);
            if (buffer[bytes_recv - 1] == '\n')
            {
                break;
            }
        }

        cap = 0;
        rewind(file);

        while ((len = getline(&line, &cap, file)) != -1)
        {
            send(client_fd, line, len, MSG_NOSIGNAL);
        }

        free(line);
        shutdown(client_fd, SHUT_WR);
        close(client_fd);
        syslog(LOG_INFO, "Closed connection from %X", ntohl(((struct sockaddr_in *)&client_addr)->sin_addr.s_addr));
    }

    return 0;
}
