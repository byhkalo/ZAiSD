//
//  main.cpp
//  ZAiSD_Lab1
//
//  Created by Konstantyn on 10/23/18.
//  Copyright © 2018 Konstantyn Bykhkalo. All rights reserved.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

/// Struct and Constants
struct Edge {
    int vertexA;
    int vertexB;
    int weight;
};
const int INFINITY = 1000000;
/// ---------------------------
/// Utilitiy Methods
/// ---------------------------
template<char delimiter>
class WordDelimitedBy : public std::string {};
/// Parse File to lines
void makeInput(std::string fileName, std::function<void (int, std::string)> readedLineLambda) {
    std::ifstream file(fileName);
    std::string line;
    int lineIndex = 0;
    
    if (!file.is_open()) { // If file isn't open
        std::cout << "Can't open file\n"; // alert
    } else {
        while(getline(file, line)) {
            readedLineLambda(lineIndex, line);
            lineIndex++;
        }
        file.close();
    }
}
/// Split String method
std::vector<std::string> splitStringBy(std::string text, std::string delimiter) {
    std::vector<std::string> result = {};
    size_t pos = 0;
    std::string token;
    while ((pos = text.find(delimiter)) != std::string::npos) {
        token = text.substr(0, pos);
        result.push_back(token);
        text.erase(0, pos + delimiter.length());
    }
    result.push_back(text);
    return result;
}
/// Output method
void clearOutput() {
    std::ofstream outfile;
    outfile.open("output.txt");
    outfile.clear();
}
void makeOutput(std::string firstLine, std::vector<int> distance) {
    std::ofstream outfile;
    outfile.open("output.txt", std::ios_base::app);
    
    outfile << "\n" + firstLine + "\n";
    int size = int(distance.size());
    for (int i = 0; i<size; i++) {
        outfile << "distance to vertex " + std::to_string(i)
        + " is " + std::to_string(distance[i]) + "\n";
    }
    outfile.close();
    std::cout << "Please check output.txt file\n";
}
/// ---------------------------
/// Table/Matrix Implementation
/// ---------------------------
/// Parse Method
std::vector<int> parseVertexLineByText(std::vector<std::string> text) {
    std::vector<int> vertextLine = {};
    for (int i = 1; i< text.size()-1; i++) {
        std::string value = text[i];
        int weight = (value == " - ") ? INFINITY : std::stoi(value);
        vertextLine.push_back(weight);
    }
    return vertextLine;
}
/// Algorithm Implementation
void tableImplementation() {
    int vertexesCount = 0, startVertex = 0;
    std::vector<std::vector<int>> tableMatrix = {};
    std::string fileName = "input_table.txt";
    makeInput(fileName, [&](int lineIndex, std::string line) mutable {
        switch (lineIndex) {
            case 0: startVertex = std::stoi(splitStringBy(line, "start vertex = ")[1]); break;
            case 1: vertexesCount = std::stoi(splitStringBy(line, "vertexes count = ")[1]); break;
            case 2: break; // Just skip line
            default: tableMatrix.push_back(parseVertexLineByText(splitStringBy(line, "|"))); break;
        }
    });
    /// Prepare Distance Vector
    std::vector<int> distance(vertexesCount, INFINITY);
    distance[startVertex] = 0;
    /// Main Algorithm
    for (int vertexIndex=0; vertexIndex < vertexesCount-1; ++vertexIndex) {
        for (int vertexA=0; vertexA < vertexesCount; ++vertexA) {
            for (int vertexB=0; vertexB < vertexesCount; ++vertexB) {
                int weight = tableMatrix[vertexB][vertexA];
                int newWeight = distance[vertexA] + weight;
                if (distance[vertexB] > newWeight) {
                    distance[vertexB] = newWeight;
                }
            }
        }
    }
    /// Output
    makeOutput("Output from Table/Matrix Function", distance);
}
/// -------------------
/// List Implementation
/// -------------------
/// Parse method
Edge parseEdgeByText(std::vector<std::string> text) {
    Edge tempEdge = Edge();
    tempEdge.vertexA = std::stoi(text[0]);
    tempEdge.vertexB = std::stoi(text[1]);
    tempEdge.weight = std::stoi(text[2]);
    return tempEdge;
}
/// Algorithm Implementation
void listImplementation() {
    int vertexesCount = 0, edgesCount = 0, startVertex = 0;
    std::vector<Edge> edges = {};
    /// Read input_list file with values
    std::string fileName = "input_list.txt";
    makeInput(fileName, [&](int lineIndex, std::string line) mutable {
        switch (lineIndex) {
            case 0: startVertex = std::stoi(splitStringBy(line, "start vertex = ")[1]); break;
            case 1: vertexesCount = std::stoi(splitStringBy(line, "vertexes count = ")[1]); break;
            default: edges.push_back(parseEdgeByText(splitStringBy(line, " "))); break;
        }
    });
    /// Prepare edges and distance vector
    edgesCount = int(edges.size());
    std::vector<int> distance(vertexesCount, INFINITY);
    distance[startVertex] = 0;
    /// Main Algorithm
    for (int vertexIndex=0; vertexIndex < vertexesCount-1; ++vertexIndex) {
        for (int edgeIndex=0; edgeIndex < edgesCount; ++edgeIndex) {
            Edge e = edges[edgeIndex];
            int newWeight = distance[e.vertexA] + e.weight;
            if (distance[e.vertexB] > newWeight) distance[e.vertexB] = newWeight;
        }
    }
    /// Output
    makeOutput("Output from List Function", distance);
}
/// ---------------------------
/// MAIN
/// ---------------------------
int main(int argc, const char * argv[]) {
    bool loop = true;
    do {
        int type;
        std::cout << "\nPlease choose variant for executing 💻\n (1) - Table Implementation\n (2) - List Implementation\n (3) - Both\n (4) - Exit\n";
        std::cin >> type;
        if (std::cin.fail()) {
            std::cout << "\nPlease use numbers: | 1️⃣ | 2️⃣ | 3️⃣ | 4️⃣ |";
            std::cin.clear();
            std::cin.ignore(INT_MAX,'\n');
            type = 0;
        }
        if (type != 4) clearOutput();
        switch (type) {
            case 1: tableImplementation(); break;
            case 2: listImplementation(); break;
            case 3: tableImplementation();
                listImplementation(); break;
            case 4: loop = false; break;
            default: break;
        }
    } while(loop);
    
    return 0;
}
