//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <algorithm>
//#include <iterator>
//
//#include "../include/sort.h"
//
//int main() {
//
//    tape::Tape<int> in("../tmp/file1.txt");
//    tape::Tape<int> out("../tmp/file1-out.txt");
//    size_t k = 3;
//    Sort<int>::sort(in, out, k);
//
//    while(!out.is_eot()) {
//        std::cout << out.read() << '\n';
//        out.left();
//    }
//    return 0;
//}
