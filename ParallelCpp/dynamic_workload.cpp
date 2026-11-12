#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <cmath>
#include <chrono>
#include <algorithm>

struct Task {
    int id;
    std::size_t iterations;
};

struct WorkerStats {
    std::size_t tasks_completed = 0;
    std::size_t total_iterations = 0;
};

double simulate(const Task& task) {
    double x = 0.1;

    for (std::size_t i = 0; i < task.iterations; ++i) {
        x = std::sin(x + 0.000001 * (i % 1000)) + 1.000001;
    }

    return x;
}


int main() {
    constexpr std::size_t NUM_TASKS = 40;

    unsigned int num_threads = std::thread::hardware_concurrency();

    if (num_threads == 0)
        num_threads = 4;

    // shared
    std::queue<Task> task_queue;
    std::mutex queue_mutex;

    std::vector<double> results(NUM_TASKS);
    std::vector<WorkerStats> workers_stats(num_threads);

    for (auto i = 0; i < NUM_TASKS; i++) {
        std::size_t workload = 100'000 * (1 + (i * 7) % 10);
        task_queue.push({
            static_cast<int>(i),
            workload
        });
    }

    std::cout << "Threads: " << num_threads << '\n';
    std::cout << "Tasks: " << NUM_TASKS << '\n';

    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> workers;

    for (auto t = 0; t < num_threads; t++) {
 
        workers.emplace_back([&, t]() {
            while(true) {
                Task task;
                {
                    std::lock_guard<std::mutex> lock(queue_mutex);

                    if (task_queue.empty())
                        break;
                    
                    task = task_queue.front();
                    task_queue.pop();
                }

                results[task.id] = simulate(task);
                workers_stats[t].tasks_completed++;
                workers_stats[t].total_iterations += task.iterations;
            }
            
        });
    }

    for (auto& worker: workers) {
        worker.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << "\nWorker statistics:\n";

    for (unsigned int t = 0; t < num_threads; ++t) {
        std::cout
            << "Worker " << t
            << " | Tasks: " << workers_stats[t].tasks_completed
            << " | Iterations: " << workers_stats[t].total_iterations
            << '\n';
    }

    std::cout << "\nExecution time: "
              << elapsed_time << " ms\n";

    std::cout << "Result of task 0: "
              << results[0] << '\n';
}