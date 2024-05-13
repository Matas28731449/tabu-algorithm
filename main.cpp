#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include <random>

using namespace std;

// Global variables
const int maxTabuSize = 10000;
const int stoppingTurn = 500;
const int start_node = 0;
int max_fitness = 0;

// Data structure to represent graph
unordered_map<string, vector<vector<int> > > read_data(const string& path) {
    unordered_map<string, vector<vector<int> > > links;
    int max_weight = 0;

    ifstream file(path);
    if (file.is_open()) {
        string line;
        while (std::getline(file, line)) {
            vector<int> link;
            int node1, node2, weight;
            sscanf(line.c_str(), "%d %d %d", &node1, &node2, &weight);
            link.push_back(node2);
            link.push_back(weight);

            if (weight > max_weight) {
                max_weight = weight;
            }

            links[to_string(node1)].push_back(link);
        }
        file.close();
    }

    return links;
}

vector<vector<string> > hill_climbing(const vector<string>& state, size_t node) {
    vector<vector<string> > neighbors;

    for (size_t i = 1; i < state.size(); ++i) {
        if (i != node) {
            vector<string> tmp_state = state;
            swap(tmp_state[i], tmp_state[node]);
            neighbors.push_back(tmp_state);
        }
    }

    return neighbors;
}

int weight_distance(int city1, int city2, const unordered_map<string, vector<vector<int> > >& graph) {
    auto neighbors = graph.at(to_string(city1));

    for (const auto& neighbor : neighbors) {
        if (neighbor[0] == city2) {
            return neighbor[1];
        }
    }

    return -1; // No such edge
}

int fitness(const vector<string>& route, const unordered_map<string, vector<vector<int> > >& graph) {
    int path_length = 0;

    for (size_t i = 0; i < route.size(); ++i) {
        int city1 = stoi(route[i]);
        int city2 = (i + 1 < route.size()) ? stoi(route[i + 1]) : stoi(route[0]);
        int dist = weight_distance(city1, city2, graph);

        if (dist == -1) {
            return max_fitness; // No such path
        }

        path_length += dist;
    }

    return path_length;
}

vector<string> tabu_search(const string& input_file_path) {
    auto graph = read_data(input_file_path);

    vector<string> s0;
    for (auto& entry : graph) {
        s0.push_back(entry.first);
    }
    std::shuffle(s0.begin(), s0.end(), std::default_random_engine(std::time(nullptr)));

    if (stoi(s0[0]) != start_node) {
        for (size_t i = 0; i < s0.size(); ++i) {
            if (stoi(s0[i]) == start_node) {
                swap(s0[0], s0[i]);
                break;
            }
        }
    }

    int max_weight = 0;
    for (auto& entry : graph) {
        for (auto& link : entry.second) {
            if (link[1] > max_weight) {
                max_weight = link[1];
            }
        }
    }
    max_fitness = (max_weight * s0.size()) + 1;

    vector<string> sBest = s0;
    int vBest = fitness(s0, graph);
    vector<string> bestCandidate = s0;
    vector<vector<string> > tabuList;
    tabuList.push_back(s0);
    bool stop = false;
    int best_keep_turn = 0;

    clock_t start_time = clock();
    while (!stop) {

        auto newCandidates = hill_climbing(bestCandidate, rand() % (bestCandidate.size() - 1) + 1);
        if (!newCandidates.empty()) {
            bestCandidate = newCandidates[0]; // Select the first candidate from hill_climbing results
}

        if (fitness(bestCandidate, graph) < vBest) {
            sBest = bestCandidate;
            vBest = fitness(sBest, graph);
            best_keep_turn = 0;
        }

        tabuList.push_back(bestCandidate);
        if (tabuList.size() > maxTabuSize) {
            tabuList.erase(tabuList.begin());
        }

        if (best_keep_turn == stoppingTurn) {
            stop = true;
        }

        ++best_keep_turn;
    }

    double exec_time = double(clock() - start_time) / CLOCKS_PER_SEC;
    cout << vBest << endl;
    return sBest;
}

int main() {
    srand(time(nullptr));
    string input_file_path = "test.txt";
    vector<string> solution = tabu_search(input_file_path);

    cout << "Optimal Solution: ";
    for (const auto& node : solution) {
        cout << node << " ";
    }
    cout << endl;

    return 0;
}
