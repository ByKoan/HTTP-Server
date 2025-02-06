// Si compilas para windows: gcc Server.c -o Server.exe -lws2_32
// Si compilas para Linux: gcc Server.c -o Server.elf

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t; // Definir socklen_t en Windows
#define CLOSE_SOCKET closesocket
#else
#include <unistd.h>
#include <arpa/inet.h>
#define CLOSE_SOCKET close
#endif

#define WEB_ROOT "\\Web"
#define DEFAULT_PORT 443
#define DEFAULT_BUFFER_SIZE 1024

// Funcion para manejar respuestas y mensajes del cliente
void handle_client(int client_socket) {
    const char *response = // Respuesta del servidor
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, World!";

    send(client_socket, response, strlen(response), 0); // Enviar respuesta al cliente
    CLOSE_SOCKET(client_socket); // Cerrar socket
}

int main() {
#ifdef _WIN32 // Si detecta que se esta ejecutando en Windows inicializa Winsock
    WSADATA wsa_data; // Estructura para almacenar informacion de Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) { // Inicializar Winsock
        fprintf(stderr, "WSAInitializacion failed: %d\n", WSAGetLastError());
        return EXIT_FAILURE;
    }
#endif

    int server_socket = socket(AF_INET, SOCK_STREAM, 0); // Crear socket del servidor
    if (server_socket == -1) { // Verificar que se creo correctamente el socket
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    // Variables y estructuras para guardar la configuracion del servidor
    struct sockaddr_in server_address;  // Estructura para guardar la direccion del servidor
    memset(&server_address, 0, sizeof(server_address)); // Poner la memoria de la direccion del servidor a 0
    server_address.sin_family = AF_INET; // Indicar la familia de direcciones (IPv4)
    server_address.sin_addr.s_addr = INADDR_ANY; // Indicar que se aceptan conexiones de cualquier direccion
    server_address.sin_port = htons(DEFAULT_PORT); // Indicar el puerto por el que escuchara el servidor

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) { // Verificar que se puedo enlazar el socket con la direccion asignada al servidor
        perror("Bind failed"); // Mostrar error de que no se pudo enlazar el socket
        CLOSE_SOCKET(server_socket); // Cerrar socket
        return EXIT_FAILURE;
    }

    if (listen(server_socket, 10) == -1) { // Verificar que se el servidor esta escuchando
        perror("Listen failed"); // Mostrar error de que el servidor no esta escuchando
        CLOSE_SOCKET(server_socket); // Cerrar socket
        return EXIT_FAILURE;
    }

    printf("Server is listening on port %d\n", DEFAULT_PORT);

    // En esta parte del codigo manejaremos las conexiones de los clientes
    while (1) {
        struct sockaddr_in client_address; 
        socklen_t client_len = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);

        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }

        printf("Connection established with %s:%d\n",
               inet_ntoa(client_address.sin_addr),
               ntohs(client_address.sin_port));

        handle_client(client_socket);
    }
    // Si no se consiguio conectar se cerrara el socket

    CLOSE_SOCKET(server_socket);

#ifdef _WIN32 // Si se esta ejecutando en Windows se limpiara Winsock
    WSACleanup();
#endif

    return EXIT_SUCCESS;
}
