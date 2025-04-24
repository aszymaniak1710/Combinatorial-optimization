#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <csignal>

struct Task {
    int JobNr, r_time, p_time, size_j;
};

bool timestop = false;

int MinStartTime(const std::vector<int>& T, int size, int rt) {
    std::vector<int> temp(T.begin(), T.end()); 
    std::sort(temp.begin(), temp.end());  
    if (temp[size-1] < rt) return rt; 
    return temp[size-1]; 
} 

int scheduleTasks(std::vector<Task>& tasks, int MaxProcs, int MaxJobs, std::ofstream& file) {
    int JobProcessed=0;
    int cmax = 0;
    std::vector<int> processors(MaxProcs, 0);

    for (int i = 0; i < MaxJobs; i++) {
        // if (timestop){
        //     std::cerr << "przekroczono limit czasu" << std::endl;
        //     break;
        // }
        int done = 0;
        int j = 0;
        int Start = MinStartTime(processors, tasks[i].size_j, tasks[i].r_time);
        file << tasks[i].JobNr << " " << Start << " " << Start + tasks[i].p_time;
        while (done < tasks[i].size_j) {
            if (processors[j] <= Start) {
                processors[j] = Start + tasks[i].p_time;
                done++;
                file << " " << j+1;
            }
            j++;
        }
        file << std::endl;
        JobProcessed++;
        cmax = *std::max_element(processors.begin(), processors.end());
    }
    std::cout << JobProcessed;
    return cmax;
}
/*
void timer(int sig){
    timestop = true;
}*/

int main(int argc, char* argv[]) {
    if(argc == 1){
    	std::cerr << "wprowadz ./szeregowanie plik_wejsciowy.txt parametr_N" <<std::endl;
        return 1;
    }
    /*
    signal(SIGALRM, timer);
    alarm(300); //w sekundach
    */
    auto start = std::chrono::high_resolution_clock::now();

    int MaxJobs = 0;
    int MaxProcs;
    std::vector<Task> tasks;
    std::ifstream file(argv[1]);
    std::ofstream file1("wynik.txt", std::ios::app);

    if (!file.is_open()) {
        std::cerr << "blad otwarcia pliku." << std::endl;
        return 1;
    }

    std::string line;
    bool Procs = false;
    while (std::getline(file, line)) {
        if (line[0] == ';') {
            if ( Procs== 0 && line.find("MaxProcs:") != std::string::npos) {
                    Procs = true;
                    MaxProcs = std::stoi(line.substr(line.find(":")+1));
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

    if (argc > 2) {
        int LinesToCompute = std::stoi(argv[2]);
        MaxJobs = LinesToCompute;
        tasks.erase(tasks.begin() + LinesToCompute + 1, tasks.end());
    }

    
    // for (int i = 0; i < MaxJobs; i++){
    //     std::cout << tasks[i].JobNr << " " << tasks[i].r_time << " " << tasks[i].p_time << " " << tasks[i].size_j << std::endl;
    // }

    std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        if (a.r_time != b.r_time) {
            return a.r_time < b.r_time;
        }
        return a.size_j < b.size_j;
    });

    for (int i = 0; i < MaxJobs; i++){
        std::cout << tasks[i].JobNr << " " << tasks[i].r_time << " " << tasks[i].p_time << " " << tasks[i].size_j << std::endl;
    }

    int cmax = scheduleTasks(tasks, MaxProcs, MaxJobs, file1);
    file1.close();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);

    std::cout << " " << cmax << " " << duration.count() << std::endl;

    return 0;
}
