#include <iostream>
#include <fstream>
#include <random>

std::string getRandomLine(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }

    // Determine file size
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Generate random position
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::streampos> dis(0, fileSize - 1);
    std::streampos randomPos = dis(gen);

    // Seek to random position
    file.seekg(randomPos);

    // Read until newline character
    std::string line;
    std::getline(file, line);

    return line;
}

int main() {
    std::string randomLine = getRandomLine("your_large_file.csv");
    std::cout << "Random line: " << randomLine << std::endl;
    return 0;
}
