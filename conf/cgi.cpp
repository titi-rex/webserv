#include <iostream>

int main() {
    // En-têtes HTTP nécessaires
    std::cout << "Content-Type: text/html\r\n\r\n";

    // Corps de la réponse HTML
    std::cout << "<!DOCTYPE html>\n";
    std::cout << "<html>\n";
    std::cout << "<head><title>Script CGI en C++</title></head>\n";
    std::cout << "<body>\n";
    std::cout << "<h1>Hello, CGI World!</h1>\n";
    std::cout << "</body>\n";
    std::cout << "</html>\n";

    return 0;
}