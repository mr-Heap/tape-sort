#pragma once

#include <algorithm>
#include <cassert>
#include <queue>
#include <type_traits>
#include <vector>

#include "tape.h"

using namespace tape;

template<typename T, typename Comparator = std::less<T>>
class Sort {
public:
    static_assert((std::is_default_constructible_v<T> &&
                   std::is_move_constructible_v<T> &&
                   std::is_copy_constructible_v<T> &&
                   std::is_copy_assignable_v<T> && std::is_destructible_v<T>));

//    using TapeType = typename std::conditional<emulated, EmulatedBinaryTape<T>, BinaryTape<T>>::type;

    template<typename TapeType>
    static void sort(TapeType & in, TapeType & out, size_t M) {
        {
            Comparator comp;


            TapeType tape_a("../tmp/tape_a.bin");
            TapeType *tape_a_ptr = &tape_a; // I don't use smart pointers because object will be destruct, and it isn't depend on ptr
            TapeType tape_b("../tmp/tape_b.bin");
            TapeType *tape_b_ptr = &tape_b;
            move_to_start(in);
            move_to_start(out);
            sort_blocks(in, tape_a, M, comp);
            const size_t N = tape_size(tape_a);
            move_to_start(tape_a);
            size_t k = M;
            while (k < N) {
                while (!tape_a_ptr->is_eot()) {
                    TapeType first("../tmp/first.bin");
                    TapeType second("../tmp/second.bin");
                    copy_to_tape(*tape_a_ptr, first, k);
                    copy_to_tape(*tape_a_ptr, second, k);
                    move_to_start(first);
                    move_to_start(second);
                    merge(first, second, *tape_b_ptr, comp, M);
                }
                k *= 2;
                move_to_start(tape_a);
                move_to_start(tape_b);
                std::swap(tape_a_ptr, tape_b_ptr);
            }
            move_to_start(*tape_a_ptr);
            while (!tape_a_ptr->is_eot()) {
                out.write(tape_a_ptr->read());
                tape_a_ptr->left();
                out.left();
            }
            move_to_start(out);
        }
        remove("../tmp/tape_a.bin");
        remove("../tmp/tape_b.bin");
        remove("../tmp/first.bin");
        remove("../tmp/second.bin");
    }

private:
    static void sort_blocks(auto &in, auto &out, size_t &M,
                            Comparator &comp) {
        std::vector<T> block;
        block.reserve(M);

        while (!in.is_eot()) {
            for (size_t i = 0; i < M && !in.is_eot(); ++i) {
                block.emplace_back(in.read());
                in.left();
            }
            std::sort(block.begin(), block.end(), comp);
            for (T &elem: block) {
                out.write(elem);
                out.left();
            }
            block.clear();
        }
    }

    static size_t tape_size(auto &tp) {
//        while (!tp.is_eot()) {
//            tp.left();
//        }
        size_t count = 0;
        while (!tp.is_start()) {
            tp.right();
            count++;
        }
        return count;
    }


    static void move_to_start(auto &tp) {
        while (!tp.is_start()) {
            tp.right();
        }
    }

    static void copy_to_tape(auto &from, auto &to, size_t &amount) {
        for (size_t i = 0; i < amount && !from.is_eot(); ++i) {
            to.write(from.read());
            to.left();
            from.left();
        }
    }


    static void merge(auto &tape_a, auto &tape_b,
                      auto &out, Comparator &comp, size_t &M) {

        std::deque<T> buffer_a, buffer_b; // two buffers of size M/2, summary M
        for (size_t i = 0; i < M / 2; ++i) {
            if (!tape_a.is_eot()) {
                buffer_a.emplace_back(tape_a.read());
                tape_a.left();
            }
            if (!tape_b.is_eot()) {
                buffer_b.emplace_back(tape_b.read());
                tape_b.left();
            }
        }


        while (!buffer_a.empty() && !buffer_b.empty()) {
            if (comp(buffer_a.front(), buffer_b.front())) {
                buffer_to_tape(tape_a, buffer_a, out);
            } else {
                buffer_to_tape(tape_b, buffer_b, out);
            }
        }
        while (!buffer_a.empty()) {
            buffer_to_tape(tape_a, buffer_a, out);
        }
        while (!buffer_b.empty()) {
            buffer_to_tape(tape_b, buffer_b, out);
        }
    }

    static void buffer_to_tape(auto &tape,
                               std::deque<T> &buffer,
                               auto &out) {
        out.write(buffer.front());
        out.left();
        buffer.pop_front();
        if (!tape.is_eot()) {
            buffer.emplace_back(tape.read());
            tape.left();
        }
    }
};