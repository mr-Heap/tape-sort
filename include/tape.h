//
// Created by marina on 10/17/23.
//

// TODO:                 std::predicate auto predict = [](int c) {return std::isdigit(c) || c == static_cast<int>('-');};

#include <utility>
#include <string>
#include <cstdio>
#include <fstream>

namespace tape {

    template <typename T>
    class InterfaceTape
    {

        // TODO: std::forward in call and check
        InterfaceTape() = default;

        // retutn value of number on runner position
        virtual T read() = 0;

        // puts value to the runner position
        virtual void write(T & value) = 0;

        // return Runner position
        virtual size_t position() = 0;

        // move tape to the left of Runner
        virtual void left(T &&arg) = 0;

        // move tape to the right of Runner
        virtual void right(T &&arg) = 0;

        ~InterfaceTape() = default;

    };

    template <typename T>
    struct Runner {             //TODO: how to make Runner protected?

        T get(std::fstream * file) {
            T result;
            file->read(reinterpret_cast<char *>(&result), sizeof result);
            file->seekg((-1) * sizeof(T));
            return result;
        };

        // TODO: if length of prev number is longer of shorter?
        void set(std::fstream * file, T & value) {
            file->write(reinterpret_cast<char *>(&value), sizeof(T));
            file->seekg((-1) * sizeof(T));
        }

    };

    template <typename T>
    class TapeBinary : public InterfaceTape<T>
    {

    public:

        explicit TapeBinary(char * file_name) {         // TODO: why explicit?
            binary_file_.open(file_name, std::ios::in | std::ios::out);
        }

        T read() override {         // TODO: check copy-optimization
            T result = runner_.get(binary_file_);
            return result;
        }

        void write(T & value) override {
            runner_.set(binary_file_, value);
            binary_file_.seekg((-1) * sizeof(T));
        }

        size_t position() override {
            return shift_;
        }

        void left() override {
            binary_file_.seekg(sizeof(T));
            ++shift_;
        }

        void right() override {             // TODO: shft < 0 || shift > size??
            binary_file_.seekg((-1) * sizeof(T));
            --shift_;
        }

        ~TapeBinary() {
            binary_file_.close(); // TODO: double close? and in class Tape
            // TODO: exception if file_ is open, program fail and file_ is still open?
        }

    protected:

        //  tape shift relative to magnetic head TODO:

        size_t shift_ = 0;

        std::fstream binary_file_;

        Runner<T> runner_;

    };

    template <typename T>
    class Tape final : public TapeBinary<T>
    {

    public:
        Tape(char * file_name = "input.txt") {  // TODO: cast string to const char *?
            file_.open(file_name, std::ios::in);
            file_to_bin(file_name, file_);
        }

        void file_to_bin(const char * file_name, std::fstream from) {

            std::string file_name_to = file_name; // TODO: bin-file name
            file_name_to += "_bin";

            std::fstream to;
            to.open(file_name_to, std::ios::out);


            while(!from.eof()) { // TODO: is it correct?
                int n;
                from >> n;
                to.write(reinterpret_cast<char *>(&n), sizeof n);

            }
            file_.close();
        }


    private:

        std::fstream file_;

    };


} // namespace tape



 // TODO: C++ documentation
