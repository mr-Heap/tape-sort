#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>


int prev(std::fstream &in) {
    //123  456
}

bool is_digit(int &symbol) {
    if (std::isdigit(symbol) || symbol == '-') {
        return true;
    }
    return false;
}

void skip(std::fstream &in, bool (*predicate)(int &)) {
    in.seekg(-1, std::ios::cur);
    int read = in.get();
    while (predicate(read)) {
        in.seekg(-2, std::ios::cur);
        read = in.get();
    }
}
//
//void left_move(std::fstream & input) {
//    input.seekg(0, std::ios::cur);//end -> cur
//    skip_left(input, is_digit);
//    skip_left(input, reinterpret_cast<bool (*)(int &)>(std::iswspace));
//}

void tape_to_bin(char * file_name) {

    std::string file_name_to = file_name;
    file_name_to += "_bin";

    std::ifstream from;
    from.open(file_name, std::ios::in);

    std::fstream to;
    to.open(file_name_to, std::ios::out);


    while(!from.eof()) {
        int n;
        from >> n;

        to.write(reinterpret_cast<char *>(&n), sizeof n);

    }
    from.close();
    to.close();

}

void read_from_bin(char * file_name) {

    std::ifstream from;
    from.open(file_name, std::ios::in);

    int n;
    from.read(reinterpret_cast<char *>(&n), sizeof n);

    std::cout << n << '\n';

    from.read(reinterpret_cast<char *>(&n), sizeof n);

    std::cout << n << '\n';

    from.read(reinterpret_cast<char *>(&n), sizeof n);

    std::cout << n << '\n';

    from.read(reinterpret_cast<char *>(&n), sizeof n);

    std::cout << n << '\n';

    from.read(reinterpret_cast<char *>(&n), sizeof n);

    std::cout << n << '\n';

    from.close();

}

int main() {

    tape_to_bin("../to-remove/to_rm.txt");

    read_from_bin("../to-remove/to_rm.txt_bin");

    std::cout << "111111111111111111111111111" << '\n';

    std::fstream bin;
    bin.open("../to-remove/to_rm.txt_bin", std::ios::in | std::ios::out);

    bin.seekg(8);

    int n = 12345;

    bin.write(reinterpret_cast<char *>(&n), sizeof n);

    n = 0;
    bin.read(reinterpret_cast<char *>(&n), sizeof n);

    std::cout << n << '\n';

    bin.close();

//    read_from_bin("../to-remove/to_rm.txt_bin");

    return 0;
}
