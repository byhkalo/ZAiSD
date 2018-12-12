//
//  main.cpp
//  ZAiSD_Lab2
//
//  Created by Konstantyn on 11/11/18.
//  Copyright © 2018 Konstantyn Bykhkalo. All rights reserved.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <queue>

/// Struct and Constants
struct Edge {
    int vertexA;
    int vertexB;
    int weight;
};
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
/// Max / Min
int max(int a, int b) {
    return a>b ? a : b;
}
int min(int a, int b) {
    return a<b ? a : b;
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
void makeOutput(std::string firstLine, std::vector<std::vector<int>> capacity,
                std::vector<int> estimatedEdges, int flowValue) {
    std::ofstream outfile;
    outfile.open("output.txt", std::ios_base::app);
    
    outfile << "\n" + firstLine + "\n";
    outfile << "Max Flow = " + std::to_string(flowValue) + "\n";
    outfile << "Vertex Count = " + std::to_string(capacity.size()) + "\n";
    int size = int(capacity.size());
    for (int i = 0; i<size; i++) {
        for (int j = 0; j<size; j++) {
            int weight = capacity[i][j];
            if (weight < 0) {
                outfile << "from vertex " + std::to_string(j)
                + " to vertex " + std::to_string(i) + " flowing " + std::to_string(weight * -1) + "\n";
            }
        }
    }
    outfile.close();
    std::cout << "Please check output.txt file\n";
}
/// ---------------------------
/// Table/Matrix Implementation
/// ---------------------------
bool isExistPathWithParent(std::vector<std::vector<int>> &capacity, int s, int t, int parent[]) {
    bool visited[capacity.size()];
    memset(visited, 0, sizeof(visited));
    std::queue <int> q;
    q.push(s);
    visited[s] = true;
    parent[s] = -1;
    //O(n^2)
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v = 0; v < capacity.size(); v++) {
            if (visited[v] == false && capacity[u][v] > 0) {
                q.push(v);
                parent[v] = u;
                visited[v] = true;
            }
        }
    }
    return (visited[t] == true);
}

/// Parse Method
std::vector<int> parseVertexLineByText(std::vector<std::string> text) {
    std::vector<int> vertextLine = {};
    for (int i = 1; i< text.size()-1; i++) {
        std::string value = text[i];
        int weight = (value == " - ") ? 0 : std::stoi(value);
        vertextLine.push_back(weight);
    }
    return vertextLine;
}
/// Algorithm Implementation
void tableImplementation() {
    int vertexesCount = 0, startVertex = 0, finishVertex = 0;
    std::vector<std::vector<int>> capacity = {};
    std::string fileName = "input_table.txt";
    makeInput(fileName, [&](int lineIndex, std::string line) mutable {
        switch (lineIndex) {
            case 0: startVertex = std::stoi(splitStringBy(line, "start vertex = ")[1]); break;
            case 1: finishVertex = std::stoi(splitStringBy(line, "destination vertex = ")[1]); break;
            case 2: break; // Just skip line
            case 3: break; // Just skip line
            default: capacity.push_back(parseVertexLineByText(splitStringBy(line, "|"))); break;
        }
    });
    vertexesCount = int(capacity[0].size());
    /// Main Algorithm
    int parent[capacity.size()];
    int max_flow = 0;
    while (isExistPathWithParent(capacity, startVertex, finishVertex, parent)) {
        int path_flow = INT_MAX;
        int tempPreviousVertex = 0;
        for (int currentVertex = finishVertex; currentVertex != startVertex;
             currentVertex = parent[currentVertex]) {
            tempPreviousVertex = parent[currentVertex];
            path_flow = min(path_flow, capacity[tempPreviousVertex][currentVertex]);
        }
        for (int currentVertex = finishVertex; currentVertex != startVertex;
             currentVertex = parent[currentVertex]) {
            tempPreviousVertex = parent[currentVertex];
            capacity[tempPreviousVertex][currentVertex] -= path_flow;
            capacity[currentVertex][tempPreviousVertex] += path_flow;
        }
        max_flow += path_flow;
    }
    /// Output
    makeOutput("Output from Table/Matrix Function", capacity, {}, max_flow);
}
/// -------------------
/// List Implementation
/// -------------------
bool isExistPathWithParentByEdges(std::vector<Edge> &edges, int s, int t, Edge parent[], int vertexCount) {
    bool visited[vertexCount];
    memset(visited, 0, vertexCount);
    std::queue <int> q;
    q.push(s);
    visited[s] = true;
    parent[s] = Edge();
    // O(m*n)
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v = 0; v < vertexCount; v++) {
            if (visited[v] == false) {
                for (int edgeIndex = 0; edgeIndex < edges.size(); edgeIndex++) {
                    Edge edge = edges[edgeIndex];
                    if (edge.vertexA == u && edge.vertexB == v && edge.weight > 0) {
                        q.push(v);
                        parent[v] = edge;
                        visited[v] = true;
                        break;
                    }
                }
            }
        }
    }
    return (visited[t] == true);
}

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
    int vertexesCount = 0, edgesCount = 0, startVertex = 0, finishVertex = 0;
    std::vector<Edge> edges = {};
    /// Read input_list file with values
    std::string fileName = "input_list.txt";
    makeInput(fileName, [&](int lineIndex, std::string line) mutable {
        switch (lineIndex) {
            case 0: startVertex = std::stoi(splitStringBy(line, "start vertex = ")[1]); break;
            case 1: finishVertex = std::stoi(splitStringBy(line, "destination vertex = ")[1]); break;
            default:
                Edge tempEdge = parseEdgeByText(splitStringBy(line, " "));
                vertexesCount = max(vertexesCount, max(tempEdge.vertexA, tempEdge.vertexB));
                edges.push_back(tempEdge); break;
        }
    });
    vertexesCount++;
    edgesCount = int(edges.size());
    
    std::vector<std::vector<int>> restoredCapacity(vertexesCount, std::vector<int>(vertexesCount, 0));
    
    /// Main Algorithm
    Edge parent[vertexesCount];
    int max_flow = 0;
    while (isExistPathWithParentByEdges(edges, startVertex, finishVertex, parent, vertexesCount)) {
        int path_flow = INT_MAX;
        for (int currentVertex = finishVertex; currentVertex != startVertex; currentVertex = parent[currentVertex].vertexA) {
            Edge tempEdge = parent[currentVertex];
            path_flow = min(path_flow, tempEdge.weight);
        }
        // O(m*n)
        for (int currentVertex = finishVertex; currentVertex != startVertex;
             currentVertex = parent[currentVertex].vertexA) {
            Edge parentEdge = parent[currentVertex];
            for (int edgeIndex = 0; edgeIndex < edgesCount; edgeIndex++) {
                Edge tempEdge = edges[edgeIndex];
                if (tempEdge.vertexA == parentEdge.vertexA && tempEdge.vertexB == parentEdge.vertexB) {
                    tempEdge.weight -= path_flow;
                    restoredCapacity[tempEdge.vertexB][tempEdge.vertexA] -= path_flow;
                    edges[edgeIndex] = tempEdge;
                    break;
                }
            }
        }
        max_flow += path_flow;
    }
    /// Output
    makeOutput("Output from LIST Function", restoredCapacity, {}, max_flow);
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
