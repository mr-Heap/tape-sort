#include <fstream>
#include <list>
#include <deque>
#include <thread>
#include <chrono>
#include <iostream>


namespace tape {
    class ITapeObserver {
    public:
        virtual void display() = 0;

    protected:
        ITapeObserver() noexcept = default;
    };

    class ITapeObservable {
    public:
        virtual void add_observer(ITapeObserver *observer) = 0;

        virtual void remove_observer(ITapeObserver *observer) = 0;

        virtual void notify_observers() = 0;

    protected:
        ITapeObservable() noexcept = default;
    };

    //Singleton class
    class Emulator final : ITapeObservable {
    public:

        Emulator(Emulator &other) = delete;

        void operator=(const Emulator &) = delete;

        static Emulator *get_instance(size_t capacity_, size_t delay_);

        static Emulator *get_instance();

        void add_observer(ITapeObserver *observer) override {
            observers_.emplace_back(observer);
            notify_observers();
        }

        void remove_observer(ITapeObserver *observer) override {
            for (auto it = observers_.begin(); it != observers_.end(); it++) {
                if (*it == observer) {
                    it = observers_.erase(it);
                }
            }
            notify_observers();
        }

        void notify_observers() override {
#ifdef _WIN32
            system("cls");
#endif
#if defined(__linux__) || defined(__APPLE__)
            system("clear");
#endif
            for (ITapeObserver *&observer: observers_) {
                observer->display();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_));
        }

    public:
        Emulator(const size_t capacity, const size_t delay) : capacity_(capacity), delay_(delay) {}

        size_t capacity_;
    protected:
        static Emulator *emulator_;
        std::list<ITapeObserver *> observers_;
        size_t delay_;
    };

    Emulator *Emulator::emulator_ = nullptr;

    Emulator *Emulator::get_instance(const size_t capacity_, const size_t delay_) {
        if (emulator_ == nullptr) {
            return new Emulator(capacity_, delay_);
        }
        return emulator_;
    }

    Emulator *Emulator::get_instance() {
        return get_instance(5, 20);
    }


    template<typename T>
    class ITape {
    public:
        // return value of number on runner position
        virtual T read() = 0;

        // puts value to the runner position
        virtual void write(T &value) = 0;

        virtual void write(T &&value) = 0;

        // return true if position is on the end of tape
        [[nodiscard]] virtual bool is_eot() = 0;

        // return true if position is on the start of tape
        [[nodiscard]] virtual bool is_start() = 0;

        // move tape to the left of Runner
        virtual void left() = 0;

        // move tape to the right of Runner
        virtual void right() = 0;

        ~ITape() = default;

    protected:
        ITape() = default;
    };

    template<typename T>
    struct Runner {
        static T get(std::fstream &file) {
            T result;
            file.read(reinterpret_cast<char *>(&result), sizeof(T));
            return result;
        };

        static void set(std::fstream &file, T &value) {
            file.write(reinterpret_cast<char *>(&value), sizeof(T));
        }
    };

    template<typename T>
    class BinaryTape : public ITape<T> {
    public:
        BinaryTape() noexcept = default;

        BinaryTape(BinaryTape<T> &&other) noexcept {
            this->binary_file_ = std::move(other.binary_file_);
        }

        explicit BinaryTape(const std::string &file_name) {
            binary_file_.open(file_name, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
        }

        explicit BinaryTape(std::string &&file_name) {
            binary_file_.open(file_name, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
        }


        T read() override {
            if (!is_eot()) {
                T result = Runner<T>::get(binary_file_);
                binary_file_.seekg((-1) * static_cast<int>(sizeof(T)), std::ios::cur);
                return result;
            }
            return T(); // RVO/NRVO
        }

        void write(T &value) override {
            Runner<T>::set(binary_file_, value);
            binary_file_.seekp((-1) * static_cast<int>(sizeof(T)), std::ios::cur);
        }

        void write(T &&value) override {
            Runner<T>::set(binary_file_, value);
            binary_file_.seekp((-1) * static_cast<int>(sizeof(T)), std::ios::cur);
        }

        [[nodiscard]] bool is_eot() override {
            std::streampos offset = binary_file_.tellg();
            bool is_eof = binary_file_.get() == -1;
            binary_file_.clear();
            binary_file_.seekg(offset, std::ios::beg);
            binary_file_.seekp(offset, std::ios::beg);
            return is_eof;
        }

        bool is_start() override {
            return binary_file_.tellg() == 0 && binary_file_.tellp() == 0;
        }

        void left() override {
            if (!is_eot()) {
                std::streampos get_pos = binary_file_.tellg() + std::streampos(sizeof(T));
                std::streampos set_pos = binary_file_.tellp() + std::streampos(sizeof(T));
                binary_file_.seekg(get_pos, std::ios::beg);
                binary_file_.seekp(set_pos, std::ios::beg);
            }
        }

        void right() override {
            if (!is_start()) {
                std::streampos get_pos = binary_file_.tellg() - std::streampos(sizeof(T));
                std::streampos set_pos = binary_file_.tellp() - std::streampos(sizeof(T));
                binary_file_.seekg(get_pos, std::ios::beg);
                binary_file_.seekp(set_pos, std::ios::beg);
            }
        }

    protected:
        std::fstream binary_file_{};
    };


    template<typename T>
    class EmulatedBinaryTape : public BinaryTape<T>, ITapeObserver {
    public:
        EmulatedBinaryTape() : BinaryTape<T>() {
            emulator_->add_observer(this);
        }

        explicit EmulatedBinaryTape(BinaryTape<T> &&other) noexcept {
            this->binary_file_ = std::move(other.binary_file_);
            emulator_->add_observer(this);
        }

        explicit EmulatedBinaryTape(const std::string &file_name) : BinaryTape<T>(file_name) {
            emulator_->add_observer(this);
        }

        explicit EmulatedBinaryTape(std::string &&file_name) : BinaryTape<T>(file_name) {
            emulator_->add_observer(this);
        }

        void display() override {
            for (size_t i = 0; i < emulator_->capacity_; i++) {
                std::cout << (i < splice_.size() ? std::to_string(splice_[i]) : "_") << " ";
            }
            std::cout << "\n";
            for (size_t i = 0; i < pos_; i++) {
                for (size_t j = 0; j <= get_n_size(splice_[i]); j++) {
                    std::cout << " ";
                }
            }
            std::cout << "^" << "\n";
        }

        void write(T &value) override {
            BinaryTape<T>::write(value);
            set(std::forward<T>(value));
        }

        void write(T &&value) override {
            BinaryTape<T>::write(value);
            set(std::forward<T>(value));
        }

        void left() override {
            if (this->is_eot()) {
                return;
            }
            BinaryTape<T>::left();
            if (!this->is_eot()) {
                if (pos_ == emulator_->capacity_ - 1) {
                    splice_.push_back(this->read());
                }
            }
            if (pos_ == emulator_->capacity_ - 1) {
                splice_.pop_front();
            }
            pos_++;
            pos_ = std::min(pos_, emulator_->capacity_ - 1);
            emulator_->notify_observers();
        }

        void right() override {
            BinaryTape<T>::right();
            if (pos_ == 0) {
                splice_.push_front(this->read());
            }
            if (pos_ == 0 && emulator_->capacity_ + 1 == splice_.size()) {
                splice_.pop_back();
            }
            if (pos_ > 0) {
                --pos_;
            }
            emulator_->notify_observers();
        }

        ~EmulatedBinaryTape() {
            emulator_->remove_observer(this);
        }

    private:
        Emulator *emulator_ = Emulator::get_instance();
        std::deque<T> splice_;
        size_t pos_ = 0;

        void set(T &&value) {
            if (pos_ < splice_.size()) {
                splice_[pos_] = value;
            } else {
                splice_.push_back(value);
            }
            emulator_->notify_observers();
        }

        size_t get_n_size(const T &number) {
            return std::to_string(number).length();
        }


    };

    template<typename T, bool emulated = false>
    class Tape : public std::conditional<emulated, EmulatedBinaryTape<T>, BinaryTape<T>>::type {
        using TapeType = typename std::conditional<emulated, EmulatedBinaryTape<T>, BinaryTape<T>>::type;
    public:
        explicit Tape(std::string &&file_name, std::ios::openmode mode = std::ios::in | std::ios::out) : TapeType() {
            file_.open(file_name, mode | std::ios::in | std::ios::out);
            raw_to_binary(file_name);
        }

        explicit Tape(std::string &file_name, std::ios::openmode mode = std::ios::in | std::ios::out) : TapeType() {
            file_.open(file_name, mode | std::ios::in | std::ios::out);
            raw_to_binary(file_name);
        }

        void flush() {
            file_.seekg(0, std::ios::beg);
            file_.seekp(0, std::ios::beg);
            while (!this->is_start()) {
                this->right();
            }
            while (!this->is_eot()) {
                file_ << this->read() << " ";
                this->left();
            }
            file_.flush();
        }

        void move_to_start() {
            while(!this->is_start()) {
                this->right();
            }
        }

        ~Tape() {
            file_.close();
        }

    private:

        std::string generate_binary_name(const std::string &file_name) { // TODO: string view?
            size_t dot = file_name.find_last_of('.');
            return file_name.substr(0, dot) + ".bin";
        }

        void raw_to_binary(std::string &file_name) {
            std::string bin_file_name = generate_binary_name(file_name);
            this->binary_file_.open(bin_file_name, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
            while (!eof()) {
                T n;
                file_ >> n;
                this->write(n);
                this->left();
            }
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

}