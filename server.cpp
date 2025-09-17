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
        return "";
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
    size_t metodo_end = request.find(' ');
    size_t path_end = request.find(' ', metodo_end + 1);
    if (metodo_end == std::string::npos || path_end == std::string::npos)
        return "HTTP/1.1 400 Bad Request\r\n\r\n";

    std::string method = request.substr(0, metodo_end);
    std::string url = request.substr(metodo_end + 1, path_end - metodo_end - 1);

    if (method != "GET")
        return "HTTP/1.1 405 Method Not Allowed\r\n\r\n";

    std::string filepath = "www" + (url == "/" ? "/index.html" : url);
    std::string body = leerArchivo(filepath);

    if (body.empty())
        return "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";

    std::string contentType = getTipoContenido(filepath);
    return "HTTP/1.1 200 OK\r\nContent-Type: " + contentType +
           "\r\nContent-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed\n";
        return 1;
    }

    if (listen(server_fd, 10) < 0)
    {
        std::cerr << "Listen failed\n";
        return 1;
    }
    std::cout << "Server running on http://localhost:8080\n";

    while (true)
    {
        int addrlen = sizeof(address);
        int client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (client_fd < 0)
        {
            std::cerr << "Accept failed\n";
            continue;
        }

        char buffer[4096] = {0};
        read(client_fd, buffer, 4096);

        std::string response = handleReq(buffer);
        write(client_fd, response.c_str(), response.size());
        close(client_fd);
    }

    close(server_fd);
    return 0;
}