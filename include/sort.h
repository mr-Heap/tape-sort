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
    static void sort(TapeType & in, TapeType & out, size_t N, size_t M) {
        static_assert((std::is_default_constructible_v<TapeType>));
        Comparator comp;
        move_to_start(in);
        if (N <= M) {
            sort_block(in, out, M, comp);
            return;
        }
//        size_t n = tape_size(in);

        std::vector<TapeType*> tapes;
        //rec_sort(in, out, N, M);
        for (size_t i = 0; i < M; ++i) {
            tapes.emplace_back(new TapeType());
            size_t size = N / M + (i < N % M);
            from_tape_to_tape(in, *tapes[i], size);
            sort(*tapes[i], *tapes[i], size, M);
        }
        auto tape_comp = [&comp](TapeType*& a, TapeType* & b) {return comp(b->read(), a->read());};
        std::priority_queue<TapeType*, std::vector<TapeType*>, decltype(tape_comp)> queue(tape_comp);
        for (auto & tape : tapes) {
            queue.push(tape);
        }
        move_to_start(out); //todo useless???
        while (!queue.empty()) {
            TapeType * tape = queue.top();
            queue.pop();
            out.write(tape->read());
            out.left();
            tape->left();
            if (!(tape->is_eot())) {
                queue.push(tape);
            }
        }
        move_to_start(out);
        for (auto & tape : tapes) {
            delete tape;
        }
        // TODO: remove (other block {})
    }

private:

    static void rec_sort(auto &in, auto &out, size_t N, size_t M) {

    }

    static void from_tape_to_tape(auto &in, auto &out, size_t n) {
        for (size_t i = 0; i < n && !in.is_eot(); ++i) {
            out.write(in.read());
            in.left();
            out.left();
        }
    }

    static void sort_block(auto &in, auto &out, size_t &M,
                            Comparator &comp) {
        std::vector<T> block;
        block.reserve(M);

        for (size_t i = 0; i < M && !in.is_eot(); ++i) {
            block.emplace_back(in.read());
            in.left();
        }
        std::sort(block.begin(), block.end(), comp);
        move_to_start(out);
        for (T &elem: block) {
            out.write(elem);
            out.left();
        }
        move_to_start(out);
    }

//    static void sort_layer(auto & tapes, auto & out, size_t M) {
//        std::priority_queue<T> queue;
//        T mx;
//        do {
//            for (auto & i : tapes) {
//                if (i->read())
//                buffer.insert(i->read());
//                if (buffer.size() == M) {
//                    buffer.erase(buffer.size() - 1);
//                }
//            }
//            mx = *(buffer.begin() + buffer.size() - 1);
//        } while (!buffer.empty());
//
//
//    }

//    static void buffer_to_tape(std::set<T> &buffer, auto &out) {
//        while(!buffer.empty()) {
//            out.write
//        }
//    }



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


};