#include <cstdlib>
#include <string>
#include <iostream>
//g++ -o sign_pdf sign_pdf.cpp
int main() {
    // Статически заданные пути
    const std::string python_script = "sign_pdf.py";
    const std::string cert_path = "certificate.p12";
    const std::string input_pdf = "test.pdf";
    const std::string output_pdf = "output-signed.pdf";

    // Формируем команду
    std::string command = "python3 " + python_script 
                        + " " + cert_path 
                        + " " + input_pdf 
                        + " " + output_pdf;

    // Выполняем
    int result = std::system(command.c_str());
    
    if (result != 0) {
        std::cerr << "Error: Failed to sign PDF!" << std::endl;
        return 1;
    }

    std::cout << "PDF signed successfully: " << output_pdf << std::endl;
    return 0;
}