
#include <iostream>
#include <thread>
#include <unordered_map>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <functional>
#include <numeric>

using namespace std;

constexpr size_t N = 10'000'000;
constexpr size_t RANGE = 1000;


unordered_map<int, int> serial_map(const vector<int>& data) {
    unordered_map<int, int> hist;
    hist.reserve(RANGE);

    for (int x : data) {
        hist[x]++;
    }

    return hist;
}

void compute_map(
    const vector<int>& data,
    unordered_map<int, int>& hist,
    size_t start,
    size_t end
) {
    for (size_t i = start; i < end; ++i) {
        hist[data[i]]++;
    }
}

unordered_map<int, int> parallel_map(
    const vector<int>& data,
    size_t num_threads
) {
    vector<unordered_map<int, int>> local_hists(num_threads);

    for (auto& hist : local_hists) {
        hist.reserve(RANGE);
    }

    size_t chunk_size =
        (data.size() + num_threads - 1) / num_threads;

    vector<jthread> workers;

    for (size_t t = 0; t < num_threads; ++t) {
        size_t start = t * chunk_size;
        size_t end = min(start + chunk_size, data.size());

        if (start >= data.size())
            break;

        workers.emplace_back(
            compute_map,
            cref(data),
            ref(local_hists[t]),
            start,
            end
        );
    }

    for (auto& worker : workers)
        worker.join();

    // Reduction / merging
    unordered_map<int, int> hist;
    hist.reserve(RANGE);

    for (const auto& local : local_hists) {
        for (const auto& [key, count] : local) {
            hist[key] += count;
        }
    }

    return hist;
}

// --------------------------------------------------
// 3. Serial vector
// --------------------------------------------------

vector<int> serial_vector(const vector<int>& data) {
    vector<int> hist(RANGE, 0);

    for (int x : data) {
        hist[x]++;
    }

    return hist;
}

void compute_vector(
    const vector<int>& data,
    vector<int>& hist,
    size_t start,
    size_t end
) {
    for (size_t i = start; i < end; ++i) {
        hist[data[i]]++;
    }
}

vector<int> parallel_vector(
    const vector<int>& data,
    size_t num_threads
) {
    vector<vector<int>> local_hists(
        num_threads,
        vector<int>(RANGE, 0)
    );

    size_t chunk_size =
        (data.size() + num_threads - 1) / num_threads;

    vector<jthread> workers;

    for (size_t t = 0; t < num_threads; ++t) {
        size_t start = t * chunk_size;
        size_t end = min(start + chunk_size, data.size());

        if (start >= data.size())
            break;

        workers.emplace_back(
            compute_vector,
            cref(data),
            ref(local_hists[t]),
            start,
            end
        );
    }

    for (auto& worker : workers)
        worker.join();

    // Reduction / merging
    vector<int> hist(RANGE, 0);

    for (size_t t = 0; t < num_threads; ++t) {
        for (size_t i = 0; i < RANGE; ++i) {
            hist[i] += local_hists[t][i];
        }
    }

    return hist;
}


template <typename Function>
double benchmark(Function&& function) {
    auto start = chrono::high_resolution_clock::now();

    auto result = function();

    auto end = chrono::high_resolution_clock::now();

    // Consume the result to prevent dead-code elimination.
    volatile long long checksum = 0;

    for (const auto& x : result) {
        if constexpr (requires { x.first; x.second; }) {
            checksum += x.second;
        } else {
            checksum += x;
        }
    }

    return chrono::duration<double, milli>(
        end - start
    ).count();
}


int main() {
    vector<int> data(N);

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, RANGE - 1);

    for (auto& x : data)
        x = dist(gen);

    size_t num_threads = thread::hardware_concurrency();

    if (num_threads == 0)
        num_threads = 4;

    num_threads = min(num_threads, N);

    cout << "Threads: " << num_threads << '\n';

    unordered_map<int, int> map_serial_result;
    unordered_map<int, int> map_parallel_result;

    vector<int> vector_serial_result;
    vector<int> vector_parallel_result;

    double t1 = benchmark([&] {
        map_serial_result = serial_map(data);
        return map_serial_result;
    });

    double t2 = benchmark([&] {
        map_parallel_result = parallel_map(data, num_threads);
        return map_parallel_result;
    });

    double t3 = benchmark([&] {
        vector_serial_result = serial_vector(data);
        return vector_serial_result;
    });

    double t4 = benchmark([&] {
        vector_parallel_result = parallel_vector(data, num_threads);
        return vector_parallel_result;
    });

    bool correct = true;

    for (size_t i = 0; i < RANGE; ++i) {
        if (map_serial_result[i] != map_parallel_result[i] ||
            map_serial_result[i] != vector_serial_result[i] ||
            map_serial_result[i] != vector_parallel_result[i]) {
            correct = false;
            break;
        }
    }

    cout << "\nCorrectness: "
         << (correct ? "PASSED" : "FAILED") << '\n';

    cout << "\n--- Benchmark Results ---\n";

    cout << "Serial unordered_map:   " << t1 << " ms\n";
    cout << "Parallel unordered_map: " << t2 << " ms\n";
    cout << "Serial vector:          " << t3 << " ms\n";
    cout << "Parallel vector:        " << t4 << " ms\n";

    cout << "\n--- Speedup ---\n";

    cout << "unordered_map: " << t1 / t2 << "x\n";
    cout << "vector:        " << t3 / t4 << "x\n";

    return 0;
}
