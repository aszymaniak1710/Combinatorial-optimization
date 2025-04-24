#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <sstream>

using namespace std::chrono;

struct Task {
    int JobNr, r_time, p_time, size_j;
};

int MinStartTime(const std::vector<int>& T, int size, int rt) {
    std::vector<int> temp(T.begin(), T.end()); // tworzy dodatkowy wektor 
    std::sort(temp.begin(), temp.end());  //sortuje procesory od najmniejszego do najwieksz
    if (temp.back() < rt) return rt; // jesli procesor o najwiekszej wartosci jest dalej mniejszy od release time to zwracamy r_t
    return temp.back(); 
}

int scheduleTasks(std::vector<Task>& tasks, int MaxProcs, int MaxJobs, std::ofstream& file) {
    int cmax = 0;
    std::vector<int> processors(MaxProcs, 0);

    for (int i = 0; i < MaxJobs; ++i) {
        int done = 0;
        int j = 0;
        int Start = MinStartTime(processors, tasks[i].size_j, tasks[i].r_time);
        //file << tasks[i].JobNr << " " << Start << " " << Start + tasks[i].p_time;
        while (done < tasks[i].size_j) {
            if (processors[j] <= Start) {
                processors[j] = Start + tasks[i].p_time;
                done++;
                //file << " " << j+1;
            }
            j++;
        }
        //file << std::endl ;

        int maxCompletionTime = *std::max_element(processors.begin(), processors.end());
        if (maxCompletionTime > cmax) {
            cmax = maxCompletionTime;
        }
    }
    return cmax;
}

int main(int argc, char* argv[]) {
    auto start = high_resolution_clock::now();

    int MaxJobs = 0;
    int MaxProcs;
    std::vector<Task> tasks;
    std::ifstream file(argv[1]);
    std::ofstream file1("czas2.txt", std::ios::app);

    if (!file.is_open()) {
        std::cerr << "Can't open file." << std::endl;
        return 1;
    }

    std::string line;
    bool Procs = false;
    while (std::getline(file, line)) {
        if (line[0] == ';') {
            if (!Procs && line.find("; MaxProcs") == 0) {
                Procs = true;
                std::cout << "joł";
                MaxProcs = std::stoi(line.substr(12));
            }
        }

        std::istringstream ss(line);
        int job, release, process, size, bin;
        if (ss >> job >> release >> bin >> process >> size) {
            if (process > 0 && size > 0 && release >= 0) {
                tasks.push_back({job, release, process, size});
                MaxJobs++;
            }
        }
    }
    file.close();

    if (argc > 3) {
        int LinesToCompute = std::stoi(argv[2]);
        MaxJobs = LinesToCompute;
        tasks.erase(tasks.begin() + LinesToCompute + 1, tasks.end());
    }

    std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        if (a.r_time != b.r_time) {
            return a.r_time < b.r_time;
        }
        return a.size_j < b.size_j;
    });

    int cmax = scheduleTasks(tasks, MaxProcs, MaxJobs, file1);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    file1 << argv[2] << " " << cmax << " " << duration.count() << std::endl;

    return 0;
}
