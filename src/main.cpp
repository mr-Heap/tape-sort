#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>

#include "../include/tape.h"

int main() {

    tape::Tape<int> t("../to-remove/to-rm.txt");

//    std::fstream bin;
//    bin.open("../to-remove/to-rm.txt", std::ios::out | std::ios::binary);

//    remove("../to-remove/to-rm.bin");
    std::cout << t.read() << '\n';
    int n = 3;
    t.write(4);
//    t.left();
    std::cout << t.read() << '\n';
  //  std::cout << t.read() << '\n';


    return 0;
}
