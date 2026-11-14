#include <iostream>
#include <llvm/Support/raw_ostream.h>

int main() {
    std::cout << "Welcome to Lubb!\n";

    llvm::outs() << "LLVM is working!\n";

    return 0;
}