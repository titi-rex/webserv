#include <iostream>
#include <string>

int main() {
    std::cout << "Content-Type: text/html\r\n\r\n";
    
    std::string name;
    char *queryString = getenv("QUERY_STRING");
    if (queryString != nullptr) {
        std::string query = queryString;
        size_t pos = query.find("name=");
        if (pos != std::string::npos) {
            name = query.substr(pos + 5); 
        }
    }
    
    std::cout << "<!DOCTYPE html>\n";
    std::cout << "<html>\n";
    std::cout << "<head><title>Script CGI en C++</title></head>\n";
    std::cout << "<body>\n";
    std::cout << "<h1>Bonjour ";
    if (!name.empty()) {
        std::cout << name;
    } else {
        std::cout << "cher visiteur";
    }
    std::cout << "!</h1>\n";
    std::cout << "</body>\n";
    std::cout << "</html>\n";
    
    return 0;
}
