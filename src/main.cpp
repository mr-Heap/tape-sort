//#include <iostream>
//#include <algorithm>
//
//#include "../include/sort.h"
//#include "../include/config.h"
//
//
//int main() {
//    Config::load("../config.ini");
//    size_t capacity = std::stoull(Config::get("Emulator", "capacity"));
//    size_t delay = std::stoull(Config::get("Emulator", "delay"));
//    Emulator<int>::set_parameters(capacity, delay);
//    tape::Tape<int> in(Config::get("Sort", "file-in"));
//    tape::Tape<int> out(Config::get("Sort", "file-out"));
//    size_t M = 3;
//    Sort<int>::sort(in, out, M);
//    while(!out.is_eot()) {
//        std::cout << out.read() << '\n';
//        out.left();
//    }
//    out.flush();
//    return 0;
//}
