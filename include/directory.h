//
// Created by marina on 11/4/23.
//
#include <string>
#include <filesystem>

class Directory {
public:
    static std::string generate_binary_name(std::string && file_name) {
        size_t dot = file_name.find_last_of('.');
        return file_name.substr(0, dot) + ".bin";
    }

    static std::string generate_out_name(std::string && file_name) {
        return file_name + ".out";
    }

    static std::string get_new_tape_name() {
        return directory_ + "tape-" + std::to_string(files_count_) + ".txt";
    }

    static void add_tape() {
        ++files_count_;
    }

    static void remove_tape() {
        --files_count_;
    }

    static void delete_tape(std::string && tape_name) {
        std::filesystem::remove(tape_name);
    }

private:
    static size_t files_count_;
    static std::string directory_;
};


size_t Directory::files_count_ = 0;
std::string Directory::directory_ = "../tmp/";
