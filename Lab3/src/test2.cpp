#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

int main(int argc, char** argv) {
    std::cout << "Started test 2 program" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::fstream out("test.txt", std::ios::app);
    for (int i = 0; i < argc; i++) {
        out << std::string_view(argv[i]) << ", ";
    }

    out << std::endl;
    out.close();

    return 1;
}
