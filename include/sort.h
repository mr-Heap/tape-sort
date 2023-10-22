//
// Created by marina on 10/17/23.
//

#include <type_traits>
#include <cassert>
#include <vector>
#include <algorithm>
#include <queue>

#include "tape.h"

using namespace tape;

template<typename T, typename Comparator = std::less<T>>
class Sort {
public:
    static_assert((std::is_default_constructible_v<T> && std::is_move_constructible_v<T>
                    && std::is_copy_constructible_v<T> && std::is_copy_constructible_v<T>
                    && std::is_copy_assignable_v<T> && std::is_destructible_v<T>));

    static void sort(Tape<T> &in, Tape<T> &out, size_t &M) {
        {
            Comparator comp;
            TapeBinary<T> tape_a("../tmp/tape_a.bin");
            TapeBinary<T>* tape_a_ptr = &tape_a; // I don't use smart pointers because object will be destruct independently, and it isn't depend on ptr
            TapeBinary<T> tape_b("../tmp/tape_b.bin");
            TapeBinary<T> * tape_b_ptr = &tape_b;
            sort_blocks(in, tape_a, M, comp);
            const size_t N = tape_size(tape_a);

            size_t k = M;
//        const size_t buf_size = M / 2;
            while (k < N) {
                while (!tape_a_ptr->is_eot()) {
                    TapeBinary<T> first("../tmp/first.bin");
                    for (size_t i = 0; i < k && !tape_a_ptr->is_eot(); ++i) {
                        first.write(tape_a_ptr->read());
                        first.left();
                        tape_a_ptr->left();
                    }
                    TapeBinary<T> second("../tmp/second.bin");
                    for (size_t i = 0; i < k && !tape_a_ptr->is_eot(); ++i) {
                        second.write(tape_a_ptr->read());
                        second.left();
                        tape_a_ptr->left();
                    }
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
        remove("../tmp/tape_a.bin");;
        remove("../tmp/tape_b.bin");
        remove("../tmp/first.bin");
        remove("../tmp/second.bin");

    }

private:

    static void sort_blocks(Tape<T> &in, TapeBinary<T> &out, size_t &M, Comparator &comp) {
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

    static void move_to_start(TapeBinary<T> &tp) {
        while (!tp.is_start()) {
            tp.right();
        }
    }

    static void merge(TapeBinary<T>& tape_a, TapeBinary<T>& tape_b, TapeBinary<T>& out, Comparator& comp, size_t& M) {

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
                write_and_update_buffer(tape_a, buffer_a, out);
            } else {
                write_and_update_buffer(tape_b, buffer_b, out);
            }
        }
        while (!buffer_a.empty()) {
            write_and_update_buffer(tape_a, buffer_a, out);
        }
        while (!buffer_b.empty()) {
            write_and_update_buffer(tape_b, buffer_b, out);
        }
    }

    static void write_and_update_buffer(TapeBinary<T> & tape, std::deque<T> & buffer, TapeBinary<T> & out) {
        out.write(buffer.front());
        out.left();
        buffer.pop_front();
        if (!tape.is_eot()) {
            buffer.emplace_back(tape.read());
            tape.left();
        }
    }

    static size_t tape_size(TapeBinary<T>& tp) {
        while(!tp.is_eot()) {
            tp.right();
        }
        size_t count = 0;
        while (!tp.is_start()) {
            tp.right();
            count++;
        }
        return count;
    }
};