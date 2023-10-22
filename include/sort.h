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

// sort for integer sign types
template<typename T, typename Comparator = std::less<T>>
class Sort {
public:
    static_assert((std::is_integral_v<T> && std::is_signed_v<T>)); // TODO

    static void sort(Tape<T> &in, Tape<T> &out, size_t &M) {
        Comparator comp;
        TapeBinary<T> current("../tmp/current.bin");
        TapeBinary<T>* cur = &current;
        TapeBinary<T> current2("../tmp/current2.bin");
        TapeBinary<T> * cur2 = &current2;
        sort_blocks(in, current, M, comp);
        const size_t N = move_to_start(current);

        size_t k = M;
        const size_t buf_size = M / 2;
        while (k < N) {  // TODO: Get tape.size()
            while (!cur->is_eot()) {
                TapeBinary<T> first("../tmp/first.bin");
                for (size_t i = 0; i < k && !cur->is_eot(); ++i) {
                    first.write(cur->read());
                    first.left();
                    cur->left();
                }
                TapeBinary<T> second("../tmp/second.bin");
                for (size_t i = 0; i < k && !cur->is_eot(); ++i) {
                    second.write(cur->read());
                    second.left();
                    cur->left();
                }
                move_to_start(first);
                move_to_start(second);
                merge(first, second, *cur2, comp, M);
            }
            k *= 2;
            move_to_start(current);
            move_to_start(current2);
            TapeBinary<T>* temp = cur;
            cur = cur2;
            cur2 = temp;
//            auto temp = std::move(current2); //TODO: Fix this shit
//            current = std::move(current2);
//            current2 = current;
        }
        move_to_start(*cur);
        while (!cur->is_eot()) {
            std::cout << cur->read() << "\n";
            cur->left();
        }
    }

private:

    static void sort_blocks(Tape<T> &in, TapeBinary<T> &out, size_t &M, Comparator &comp) {
        std::vector<T> block;
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

    static size_t move_to_start(TapeBinary<T> &tp) {
        size_t count = 0;
        while (!tp.is_start()) {
            tp.right();
            count++;
        }
        return count;
    }

    static void merge(TapeBinary<T>& tape_a, TapeBinary<T>& tape_b, TapeBinary<T>& out, Comparator& comp, size_t& M) {

        std::queue<T> buffer_a, buffer_b; // two buffers of size M/2, summary M
        for (size_t i = 0; i < M / 2; ++i) {
            if (!tape_a.is_eot()) {
                buffer_a.emplace(tape_a.read());
                tape_a.left();
            }
            if (!tape_b.is_eot()) {
                buffer_b.emplace(tape_b.read());
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

    static void write_and_update_buffer(TapeBinary<T> & tape, std::queue<T> & buffer, TapeBinary<T> & out) {
        out.write(buffer.front());
        out.left();
        buffer.pop();
        if (!tape.is_eot()) {
            buffer.emplace(tape.read());
            tape.left();
        }
    }
};