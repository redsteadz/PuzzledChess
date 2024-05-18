#include <vector>
#ifndef DATABASE_H
#define DATABASE_H
#include <ctime>
#include <fstream>
#include <iostream>

using namespace std;

class Filer {
public:
  static string ReadRandomLineFromFile() {
    ifstream file("./assets/lichess_db_puzzle.csv");
    if (!file.is_open()) {
      std::cerr << "Error: Unable to open file "
                << "Database" << std::endl;
      return "";
    }
    int totalLines = 0;
    string line;
    while (std::getline(file, line)) {
      totalLines++;
    }

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    int randomLineNumber =
        std::rand() % totalLines + 1;

    file.clear();
    file.seekg(0, std::ios::beg);

    int currentLine = 0;
    while (std::getline(file, line)) {
      currentLine++;
      if (currentLine == randomLineNumber) {
        file.close();
        return line;
      }
    }

    file.close();
    return ""; 
  }
  static pair<string, string> ParseLine(){
    std::string line = ReadRandomLineFromFile();
    std::string delimiter = ",";
    // Breaking line into pieces
    size_t pos = 0;
    std::string token;
    vector<string> pieces;
    while ((pos = line.find(delimiter)) != std::string::npos) {
      token = line.substr(0, pos);
      pieces.push_back(token);
      line.erase(0, pos + delimiter.length());
    }
    return make_pair(pieces[1], pieces[2]);
  }
};

#endif // !DATABASE_H
