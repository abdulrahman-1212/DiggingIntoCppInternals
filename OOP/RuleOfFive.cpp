
#include <iostream>
#include <cstring>
#include <cstddef>

class String {
private:
    char* data;
    size_t length;
    size_t capacity;

public:
    // 1. Constructor
    String(const char* str = "") {
        if (str == nullptr)
            str = "";

        length = std::strlen(str);
        capacity = length;

        data = new char[capacity + 1];
        std::strcpy(data, str);

        std::cout << "Constructor\n";
    }

    // 2. Copy constructor (deep copy)
    String(const String& other) {
        length = other.length;
        capacity = other.capacity;

        data = new char[capacity + 1];

        if (other.data != nullptr)
            std::memcpy(data, other.data, length + 1);
        else
            data[0] = '\0';

        std::cout << "Copy constructor\n";
    }

    // 3. Copy assignment operator (deep copy)
    String& operator=(const String& other) {
        std::cout << "Copy assignment\n";

        // Prevent self-assignment
        if (this == &other)
            return *this;

        // Allocate new memory before deleting old memory
        char* new_data = new char[other.capacity + 1];

        if (other.data != nullptr)
            std::memcpy(new_data, other.data, other.length + 1);
        else
            new_data[0] = '\0';

        delete[] data;

        data = new_data;
        length = other.length;
        capacity = other.capacity;

        return *this;
    }

    // 4. Move constructor (transfer ownership)
    String(String&& other) noexcept {
        std::cout << "Move constructor\n";

        data = other.data;
        length = other.length;
        capacity = other.capacity;

        // Leave source in a valid empty state
        other.data = nullptr;
        other.length = 0;
        other.capacity = 0;
    }

    // 5. Move assignment operator (transfer ownership)
    String& operator=(String&& other) noexcept {
        std::cout << "Move assignment\n";

        // Prevent self-move
        if (this == &other)
            return *this;

        // Release current resource
        delete[] data;

        // Steal source resource
        data = other.data;
        length = other.length;
        capacity = other.capacity;

        // Leave source in a valid empty state
        other.data = nullptr;
        other.length = 0;
        other.capacity = 0;

        return *this;
    }

    // 6. Destructor
    ~String() {
        std::cout << "Destructor\n";
        delete[] data;
    }

    void print() const {
        if (data != nullptr)
            std::cout << data << '\n';
        else
            std::cout << '\n';
    }
};

int main() {
    String s1("Hello");

    // Copy constructor
    String s2 = s1;

    // Copy assignment
    String s3("World");
    s3 = s1;

    // Move constructor
    String s4 = std::move(s1);

    // Move assignment
    String s5("Temporary");
    s5 = std::move(s2);

    s1.print(); // Empty: moved-from object
    s2.print(); // Empty: moved-from object
    s3.print(); // Hello
    s4.print(); // Hello
    s5.print(); // Hello

    return 0;
}
