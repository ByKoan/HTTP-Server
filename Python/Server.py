import socket

def start_server(host='127.0.0.1', port=8080): # Funcion que iniciara el servidor
    
    # Crear un socket TCP
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Ponemos el servidor a escuchar peticiones 
    server_socket.bind((host, port))
    server_socket.listen(5)  # Escuchar hasta 5 conexiones simultáneas
    print(f"Servidor HTTP ejecutándose en http://{host}:{port}")

    while True:
        client_socket, client_address = server_socket.accept()
        print(f"Conexión recibida de {client_address}")

        # Leer la solicitud del cliente
        request = client_socket.recv(1024).decode('utf-8')
        print(f"Solicitud recibida:\n{request}")

        # Preparar la respuesta HTTP
        response = (
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "\r\n"
            """
            <html>
                <head>
                    <title>Servidor HTTP</title>
                </head>
                <body>
                    <h1>¡Hola mundo!</h1>
                </body>
            </html>
            """)

        # Enviar la respuesta al cliente
        client_socket.sendall(response.encode('utf-8'))
        client_socket.close()

# Iniciar el servidor
if __name__ == "__main__":
    start_server()
