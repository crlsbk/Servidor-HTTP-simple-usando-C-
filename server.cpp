#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

bool extensionEs(const string &str, const string &terminacion)
{
    return str.size() >= terminacion.size() &&
           str.compare(str.size() - terminacion.size(), terminacion.size(), terminacion) == 0;
}

string leerArchivo(const string &ruta)
{
    ifstream archivo(ruta, ios::binary);
    if (!archivo.is_open())
    {
        cout << " No se pudo abrir el archivo: " << ruta << endl;
        return "";
    }
    cout << " Archivo abierto correctamente: " << ruta << endl;
    stringstream ss;
    ss << archivo.rdbuf();
    return ss.str();
}

string getTipoContenido(const string &ruta)
{
    if (extensionEs(ruta, ".html"))
        return "text/html";
    if (extensionEs(ruta, ".css"))
        return "text/css";
    if (extensionEs(ruta, ".js"))
        return "application/javascript";
    if (extensionEs(ruta, ".png"))
        return "image/png";
    if (extensionEs(ruta, ".jpg") || extensionEs(ruta, ".jpeg"))
        return "image/jpeg";
    return "text/plain";
}

string handleReq(const string &request)
{
    cout << " Nuevapetición recibida:\n"
         << request << endl;
    size_t metodo_end = request.find(' ');
    size_t ruta_end = request.find(' ', metodo_end + 1);
    if (metodo_end == string::npos || ruta_end == string::npos)
    {
        cout << " Petición mal hecha" << endl;
        return "HTTP/1.1 400 Bad Request\r\n\r\n";
    }

    string metodo = request.substr(0, metodo_end);
    string url = request.substr(metodo_end + 1, ruta_end - metodo_end - 1);

    cout << " Método: " << metodo << ", URL: " << url << endl;

    if (metodo != "GET")
    {
        cout << " Metodo no permitido: " << metodo << endl;
        return "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
    }

    string rutaArch = "src/" + (url == "/" ? "/index.html" : url);
    cout << " Ruta de archivo solicitada: " << rutaArch << endl;
    string body = leerArchivo(rutaArch);

    if (body.empty())
    {
        cout << " Archivo no encontrado o vació: " << rutaArch << endl;
        return "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
    }

    string contentType = getTipoContenido(rutaArch);
    cout << " Tipo de contenido: " << contentType << ", Tamaño: " << body.size() << endl;
    return "HTTP/1.1 200 OK\r\nContent-Type: " + contentType +
           "\r\nContent-Length: " + to_string(body.size()) + "\r\n\r\n" + body;
}

int main()
{
    cout << " Iniciando servidor..." << endl;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        cerr << "Error al crear socket" << endl;
        return 1;
    }

    sockaddr_in direccion{};
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&direccion, sizeof(direccion)) < 0)
    {
        cerr << "Error con bind" << endl;
        return 1;
    }

    if (listen(server_fd, 10) < 0)
    {
        cerr << "Error con listen" << endl;
        return 1;
    }
    cout << " Servidor corriendo en http://localhost:8080" << endl;

    while (true)
    {
        int dirlen = sizeof(direccion);
        int client_fd = accept(server_fd, (struct sockaddr *)&direccion, (socklen_t *)&dirlen);
        if (client_fd < 0)
        {
            cerr << "Error con accept" << endl;
            continue;
        }

        cout << " Cliente conectado" << endl;
        char buffer[4096] = {0};
        ssize_t bytes_leidos = read(client_fd, buffer, 4096);
        if (bytes_leidos <= 0)
        {
            cout << " No se leyeron datos del cliente" << endl;
            close(client_fd);
            continue;
        }

        string response = handleReq(buffer);
        write(client_fd, response.c_str(), response.size());
        cout << " Respuesta enviada, cerrando conexión con cliente" << endl;
        close(client_fd);
    }

    close(server_fd);
    cout << " Servidor detenido" << endl;
    return 0;
}