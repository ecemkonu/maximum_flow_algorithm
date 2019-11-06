/*****************
 * Ayser Ecem Konu
 * 150160711
 * **************/
#include <iostream>
#include <fstream>
#include <climits>
#include <queue>
#include <cstring>
#include <numeric>

using namespace std;

int **readFromFile(const string &filename, int &size, int *&won) {
    fstream file;
    file.open(filename);
    file >> size;
    won = new int[size];
    int **matches = new int *[size];
    for (int i = 0; i < size; i++)
        file >> won[i];
    for (int i = 0; i < size; i++) {
        matches[i] = new int[size];
        for (int j = 0; j < size; j++)
            file >> matches[i][j];
    }
    file.close();
    return matches;
}

int *remainingMatches(int **&matches, int arrSize) {
    int *remArr = new int[arrSize];
    for (int i = 0; i < arrSize; i++) {
        remArr[i] = 0;
        for (int j = 0; j < arrSize; j++) {
            remArr[i] += matches[i][j];
        }
    }
    return remArr;
}

int max(int *&won, int arrSize, int index) {
    int maxVal = 0;
    for (int i = 0; i < arrSize; i++) {
        if (index != i) {
            if (maxVal < won[i]) {
                maxVal = won[i];
            }
        }
    }
    return maxVal;
}

int **createGraph(int **&matches, int *&won, int *&remArr, int estIndex, int arrSize) {
    int **graph;
    int combination = (arrSize - 1) * (arrSize - 2) / 2;
    int gSize = combination + arrSize + 1; //combination of remaining teams & teams & s + t.
    graph = new int *[gSize];
    for (int i = 0; i < gSize; i++) {
        graph[i] = new int[gSize];
        for (int j = 0; j < gSize; j++) {
            graph[i][j] = 0;
        }
    }
    int p = 0;
    for (int i = 0; i < arrSize; i++) {
        if (i == estIndex)
            continue;
        for (int j = i + 1; j < arrSize; j++) {
            if (j == estIndex)
                continue;
            graph[0][++p] = matches[i][j];

        }
    }
    p=0;
        for(int i = combination+1; i<gSize-1; i++)
            for(int j=i+1; j<gSize-1; j++ )
            {
                graph[++p][i] = INT16_MAX;
                graph[p][j] = INT16_MAX;
            }

    //for (int i = 0; i < arrSize - 1; i++) {
    //    for (int k = 1; k < combination + 1; k++) {
     //       graph[k][1 + combination + i] = INT16_MAX;
     //   }
    //}
    int limit = won[estIndex] + remArr[estIndex];
    for (int i = 0; i < arrSize; i++) {
        if (i == estIndex)
            continue;
        if (i < estIndex)
            graph[i + 1 + combination][gSize - 1] = limit - won[i];
        else {
            int j = i - 1;
            graph[j + 1 + combination][gSize - 1] = limit - won[i];

        }
    }
    return graph;
}

bool BFS(int **&graph, int arrSize, int sourceIndex, int *&parent) {
    bool *visited = new bool[arrSize];
    queue<int> myqueue;
    visited[sourceIndex] = true;
    parent[sourceIndex] = -1;
    myqueue.push(sourceIndex);
    while (!myqueue.empty()) {
        sourceIndex = myqueue.front();
        myqueue.pop();
        for (int i = 0; i < arrSize; i++) {
            if (!visited[i] && graph[sourceIndex][i] > 0) {
                myqueue.push(i);
                parent[i] = sourceIndex;
                visited[i] = true;
            }
        }
    }
    return visited[arrSize - 1]; //check whether the end point(t, known as sink in our case) has any connection with source.
    //sink = arrSize-1.
}

int ford_fulkerson(int **&graph, int arrSize, int comSize) {
    int source = 0; //first index to begin, s(source in graph representation.)
    int v, u;
    int **residualGraph;
    residualGraph = new int *[comSize];
    for (int i = 0; i < comSize; i++) {
        residualGraph[i] = new int[comSize];
        memcpy(residualGraph[i], graph[i], comSize * sizeof(int));
    }
    int *parent = new int[comSize];
    int maxFlow = 0;
    while (BFS(residualGraph, comSize, source, parent)) {
        int pathFlow = INT16_MAX;
        for (v = comSize - 1; v != source; v = parent[v]) {
            u = parent[v];
            pathFlow = min(pathFlow, residualGraph[u][v]);
        }
        for (v = comSize - 1; v != source; v = parent[v]) {
            u = parent[v];
            residualGraph[u][v] -= pathFlow;
            residualGraph[v][u] += pathFlow;
        }
        maxFlow += pathFlow;
    }
    for(int i=0; i<comSize;i++)
    {
        delete[] residualGraph[i];
    }
    delete[] residualGraph;
    return maxFlow;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Please provide input file name as argument" << endl;
        return -1;
    }

    int arrSize = 0;
    int *won;
    int **matches = readFromFile(argv[1], arrSize, won);
    int *outArr = new int[arrSize]{0};
    int **guessed[arrSize];
    int remMat = 0;
    int maxFlow[arrSize];
    int *remArr = remainingMatches(matches, arrSize);
    int comSize = (arrSize - 1) * (arrSize - 2) / 2 + arrSize + 1;
    for (int i = 0; i < arrSize; i++) {
        int maxVal = max(won, arrSize, i);
        if (maxVal > won[i] + remArr[i]) { //eliminate teams that have no chance of winning
            outArr[i] = 0;
            maxFlow[i] = -200;
        }
        else {
            outArr[i] = 1; //fix later
            guessed[i] = createGraph(matches, won, remArr, i, arrSize);
            maxFlow[i] = ford_fulkerson(guessed[i], arrSize, comSize);
            remMat = 0;
            for (int jj = 0; jj < arrSize; jj++) {
                remMat += remArr[jj];
            }
            remMat /= 2;
            remMat -= remArr[i];
            cout << "Remaining matches " << remMat << endl; //number of matches left to play
            if (maxFlow[i] < remMat)
                outArr[i] = 0;
        }
    }
    for (int k = 0; k < arrSize; k++) {
        cout << "Maximum flows: " << remArr[k] << endl; //remaining holds max flow values.
        for (int i = 0; i < comSize; i++) {
            for (int j = 0; j < comSize; j++) {
                if (outArr[k] > 0)
                    cout << guessed[k][i][j] << " ";
            }
            cout << endl;
        }
    }
    if (argc == 3) {
        ofstream file(argv[2]);
        if (file.is_open()) {
            cout << "Success: \n";
            for (int i = 0; i < arrSize; i++)
                file << outArr[i];
            file.close();
        }
        else {
            cout << "Couldn't create file" << endl;
        }
    }
    for (int i = 0; i < arrSize; i++) {
        for (int j = 0; j < comSize; j++)
            if(maxFlow[i]!= -200)
            {
            delete[] guessed[i][j];
            }
        delete guessed[i];
        delete matches[i];
    }
    delete[] matches;
    delete won;
    delete outArr;
    delete remArr;
    return 0;
}