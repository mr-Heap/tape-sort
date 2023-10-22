//
// Created by marina on 10/17/23.
//
/**
 *
 *
 */
#include <utility>
#include <string>
#include <cstdio>
#include <fstream>

namespace tape {

    template<typename T>
    class InterfaceTape {

    public:
        InterfaceTape() = default;

        // return value of number on runner position
        virtual T read() = 0;

        // puts value to the runner position
        virtual void write(T &value) = 0;
        virtual void write(T &&value) = 0;

        // return true if position is on the end of tape
        [[nodiscard]] virtual bool is_eot() = 0;

        // return true if position is on the start of tape
        [[nodiscard]] virtual bool is_start() = 0;

        // move tape to the left of Runner. If runner is on the end of tape, left() will do nothing
        virtual void left() = 0;

        // move tape to the right of Runner. If runner is on the start of tape, right() will do nothing
        virtual void right() = 0;

        ~InterfaceTape() = default;

    };

    template<typename T>
    struct Runner {

        static T get(std::fstream &file) {
            T result;
            file.read(reinterpret_cast<char *>(&result), sizeof result);
            return result;
        };

        static void set(std::fstream &file, T &value) {
            file.write(reinterpret_cast<char *>(&value), sizeof(T));
        }

    };

    template<typename T>
    class TapeBinary : public InterfaceTape<T> {

    public:

        TapeBinary() noexcept = default;

        TapeBinary(TapeBinary<T> &&other) noexcept {
            this->binary_file_ = std::move(other.binary_file_);
        }


        explicit TapeBinary(const std::string &file_name) {
            binary_file_.open(file_name, std::ios::in | std::ios::out | std::ios::trunc);
        }

        explicit TapeBinary(std::string &&file_name) {
            binary_file_.open(file_name, std::ios::in | std::ios::out | std::ios::trunc);
        }

        TapeBinary<T> &operator=(TapeBinary<T> const &other) = delete;

        TapeBinary<T> &operator=(TapeBinary<T> &&other) noexcept {
            this->binary_file_ = std::move(other.binary_file_);
            return *this;
        };


        T read() override {
            T result = Runner<T>::get(binary_file_);
            right();
            return result;
        }

        void write(T &value) override {
            Runner<T>::set(binary_file_, value);
            right();
        }

        void write(T &&value) override {
            Runner<T>::set(binary_file_, value);
            right();
        }

        [[nodiscard]] bool is_start() override {
            return binary_file_.tellg() == 0;
        }

        //Fix fstream.eof() doesn't return true if method get() equals -1 was not written
        [[nodiscard]] bool is_eot() override {
            std::streampos offset = binary_file_.tellg();
            bool is_eof = binary_file_.get() == -1;
            binary_file_.clear();
            binary_file_.seekg(offset);
            return is_eof;
        }

        void left() override {
            if (!is_eot()) {
                binary_file_.seekg(sizeof(T), std::ios::cur);
            }
        }

        void right() override {
            if(!is_start()) {
                binary_file_.seekg((-1) * static_cast<int>(sizeof(T)), std::ios::cur);
            }
        }

        ~TapeBinary() = default;

    protected:
        std::fstream binary_file_;

    };

    template<typename T>
    class Tape final : public TapeBinary<T> {
    public:
        explicit Tape(std::string &&file_name, std::ios::openmode mode) : TapeBinary<T>() {
            file_.open(file_name, mode);
            move_to_binary_file(generate_binary_name(file_name));
            file_.close();
        }

        explicit Tape(std::string &file_name, std::ios::openmode mode) : TapeBinary<T>() {
            file_.open(file_name, mode);
            move_to_binary_file(generate_binary_name(file_name));
            file_.close();
        }

        ~Tape() = default;


    private:
        std::string generate_binary_name(const std::string &file_name) {
            size_t dot = file_name.find_last_of('.');
            return file_name.substr(0, dot) + ".bin";
        }

        void move_to_binary_file(const std::string &bin_file_name) {
            this->binary_file_.open(bin_file_name, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
            while (!eof()) {
                T n;
                file_ >> n;
                this->binary_file_.write(reinterpret_cast<char *>(&n), sizeof n);
            }
            this->binary_file_.seekg(0);
        }

        bool eof() {
            std::streampos offset = file_.tellg();
            bool is_eof = file_.get() == -1;
            file_.clear();
            file_.seekg(offset);
            return is_eof;
        }


    private:

        std::fstream file_;
    };


} // namespace tape



// TODO: C++ documentation
