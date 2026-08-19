// eisenhower.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

struct Task {
    string id;
    string title;
    string description;
    int quadrant;
    string created;
    bool completed;
};

class EisenhowerApp {
private:
    vector<Task> tasks;
    string dataFile = "tasks.json";

    string generateId() {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, 15);
        stringstream ss;
        for (int i = 0; i < 8; i++) {
            ss << hex << dis(gen);
        }
        return ss.str();
    }

    void load() {
        ifstream f(dataFile);
        if (!f.is_open()) return;
        json j;
        f >> j;
        for (auto& item : j) {
            Task t;
            t.id = item["id"];
            t.title = item["title"];
            t.description = item.value("description", "");
            t.quadrant = item["quadrant"];
            t.created = item.value("created", "");
            t.completed = item.value("completed", false);
            tasks.push_back(t);
        }
    }

    void save() {
        json j = json::array();
        for (auto& t : tasks) {
            json item;
            item["id"] = t.id;
            item["title"] = t.title;
            item["description"] = t.description;
            item["quadrant"] = t.quadrant;
            item["created"] = t.created;
            item["completed"] = t.completed;
            j.push_back(item);
        }
        ofstream f(dataFile);
        f << setw(2) << j << endl;
    }

public:
    EisenhowerApp() { load(); }

    void addTask(string title, string desc, int quadrant) {
        Task t;
        t.id = generateId();
        t.title = title;
        t.description = desc;
        t.quadrant = quadrant;
        time_t now = time(nullptr);
        t.created = string(ctime(&now));
        t.created.pop_back();
        t.completed = false;
        tasks.push_back(t);
        save();
        cout << "Task added: " << t.title << " (ID: " << t.id << ")" << endl;
    }

    void deleteTask(string id) {
        for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            if (it->id == id) {
                tasks.erase(it);
                save();
                cout << "Task " << id << " deleted." << endl;
                return;
            }
        }
        cout << "Task " << id << " not found." << endl;
    }

    void moveTask(string id, int newQuadrant) {
        for (auto& t : tasks) {
            if (t.id == id) {
                t.quadrant = newQuadrant;
                save();
                cout << "Task " << id << " moved to quadrant " << newQuadrant << "." << endl;
                return;
            }
        }
        cout << "Task " << id << " not found." << endl;
    }

    void listTasks(int quadrant = 0) {
        vector<Task> filtered;
        for (auto& t : tasks) {
            if (quadrant == 0 || t.quadrant == quadrant) {
                filtered.push_back(t);
            }
        }
        if (filtered.empty()) {
            cout << "No tasks found." << endl;
            return;
        }
        string labels[] = {"Urgent & Important", "Not Urgent but Important",
                           "Urgent but Not Important", "Not Urgent & Not Important"};
        string colors[] = {"\033[31m", "\033[32m", "\033[33m", "\033[37m"};
        cout << "\nEisenhower Matrix" << endl;
        cout << string(60, '=') << endl;
        for (int q = 1; q <= 4; q++) {
            cout << colors[q-1] << labels[q-1] << "\033[0m" << endl;
            bool has = false;
            for (auto& t : filtered) {
                if (t.quadrant == q) {
                    cout << "  [" << t.id << "] " << t.title << " - "
                         << (t.description.empty() ? "No description" : t.description) << endl;
                    has = true;
                }
            }
            if (!has) cout << "  (empty)" << endl;
            if (q < 4) cout << string(40, '-') << endl;
        }
        cout << string(60, '=') << endl;
    }

    void stats() {
        int counts[5] = {0};
        for (auto& t : tasks) counts[t.quadrant]++;
        string labels[] = {"Urgent & Important", "Not Urgent but Important",
                           "Urgent but Not Important", "Not Urgent & Not Important"};
        cout << "Eisenhower Statistics" << endl;
        for (int q = 1; q <= 4; q++) {
            cout << labels[q-1] << ": " << counts[q] << " tasks" << endl;
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: eisenhower <command> [options]" << endl;
        return 1;
    }
    EisenhowerApp app;
    string cmd = argv[1];
    if (cmd == "add") {
        string title, desc = "";
        int quadrant = 1;
        for (int i = 2; i < argc; i++) {
            string arg = argv[i];
            if (arg == "-t" && i+1 < argc) title = argv[++i];
            else if (arg == "-d" && i+1 < argc) desc = argv[++i];
            else if (arg == "-q" && i+1 < argc) quadrant = stoi(argv[++i]);
        }
        app.addTask(title, desc, quadrant);
    } else if (cmd == "list") {
        int quadrant = 0;
        for (int i = 2; i < argc; i++) {
            if (string(argv[i]) == "-q" && i+1 < argc) quadrant = stoi(argv[++i]);
        }
        app.listTasks(quadrant);
    } else if (cmd == "move") {
        string id = "";
        int q = 0;
        for (int i = 2; i < argc; i++) {
            if (string(argv[i]) == "--id" && i+1 < argc) id = argv[++i];
            if (string(argv[i]) == "-q" && i+1 < argc) q = stoi(argv[++i]);
        }
        app.moveTask(id, q);
    } else if (cmd == "delete") {
        string id = "";
        for (int i = 2; i < argc; i++) {
            if (string(argv[i]) == "--id" && i+1 < argc) id = argv[++i];
        }
        app.deleteTask(id);
    } else if (cmd == "stats") {
        app.stats();
    } else {
        cerr << "Unknown command." << endl;
    }
    return 0;
}
