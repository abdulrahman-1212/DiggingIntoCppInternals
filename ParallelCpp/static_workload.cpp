#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>


void square_chunk(const std::vector<double>& input, std::vector<double>& output, size_t start, size_t end) {
    for (auto i = start; i < end; i++) {
        output[i] = input[i] * input[i];
    }
}



int main() {
    const size_t N = 16'000'000;

    const std::vector<double> input(N, 2);
    std::vector<double> output(N);
    unsigned int num_threads = std::thread::hardware_concurrency();

    size_t chunk_size = (N + num_threads - 1) / num_threads;

    std::cout << "Chunck size: " << chunk_size << std::endl;

    std::vector<std::thread> workers;

    // Distributed workload
    for (auto i = 0; i < num_threads; i++) {
        size_t start = i * chunk_size;
        size_t end = std::min(start + chunk_size, N);

        if (start >= N) 
            break;

        // workers.emplace_back(
        //     square_chunk,
        //     std::cref(input),
        //     std::ref(output),
        //     start,
        //     end
        // );

        workers.emplace_back([&, start, end]() {
            square_chunk(input, output, start, end);
        });
    }

    for (auto& worker: workers) {
        worker.join();
    }


    std::cout << "Result: " << output[0] << std::endl;
}

