#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>

void PrintValue(int value) {
    std::cout << value << " ";
}

void ForEach(const std::vector<int>& values, void(*func)(int)) {
    for (auto value: values) {
        func(value);
    }

    std::cout << std::endl;
}

void ForEach(const std::vector<int>& values, const std::function<void(int)>& func) {
    for (auto value: values) {
        func(value);
    }

    std::cout << std::endl;
}

int main() {
    std::vector<int> values = {1, 2, 3, 4, 5};

    auto it = std::find_if(values.begin(), values.end(), [](int value) { return value > 3; });
    std::cout << *it << std::endl;

    ForEach(values, PrintValue);
    int n = 5;
    // ForEach(values, [](int value) { std::cout << value*value << " "; });
}