#pragma once

#include <algorithm>
#include <iostream>
#include <deque>
#include <list>
#include <thread>

template<typename T>
class Emulator;


template<typename T>
class Keeper {

public:
    Keeper() {
        Emulator<T>::addKeeper(this);
    }

    void move_left() {
        if (pos_ == 0 && capacity_ + 1 == splice_.size()) {
            splice_.pop_back();
        }
        if (pos_ > 0) {
            --pos_;
        }
        Emulator<T>::show();
    }

    void move_left(T &&prev) {
        if (pos_ == 0) {
            splice_.push_front(prev);
        }
        move_left();
    }

    void move_right() {
        if (pos_ == capacity_ - 1) {
            splice_.pop_front();
        }
        pos_++;
        pos_ = std::min(pos_, capacity_ - 1);
        Emulator<T>::show();
    }

    void move_right(T &&next) {
        if (pos_ == capacity_ - 1) {
            splice_.push_back(next);
        }
        move_right();
    }

    void set(T &&value) {
        if (pos_ < splice_.size()) {
            splice_[pos_] = value;
        } else {
            splice_.push_back(value);
        }
        Emulator<T>::show();
    }

    void display() {
        for (size_t i = 0; i < capacity_; i++) {
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

    ~Keeper() {
        Emulator<T>::deleteKeeper(this);
    }

private:
    std::deque<T> splice_;
    const size_t capacity_ = 10;
    size_t pos_ = 0;

    size_t get_n_size(const T &number) { return std::to_string(number).length(); }
};


template<typename T>
class Emulator {
public:
    static void addKeeper(Keeper<T> *keeper);

    static void deleteKeeper(Keeper<T> *del_keeper);

    static void show();

private:
    static std::list<Keeper<T> *> keepers_;
};

template<typename T>
std::list<Keeper<T> *> Emulator<T>::keepers_;


template<typename T>
void Emulator<T>::addKeeper(Keeper<T> *keeper) {
    Emulator<T>::keepers_.push_back(keeper);
    show();
}

template<typename T>
void Emulator<T>::deleteKeeper(Keeper<T> *del_keeper) {
    for (auto it = keepers_.begin(); it != keepers_.end(); it++) {
        if (*it == del_keeper) {
            it = keepers_.erase(it);
        }
    }
    show();
}

template<typename T>
void Emulator<T>::show() {
#ifdef _WIN32
    system("cls");
#endif
#if defined(__linux__) || defined(__APPLE__)
    system("clear");
#endif
    for (Keeper<T> *&keeper: keepers_) {
        keeper->display();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
}



