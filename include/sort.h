//
// Created by marina on 10/17/23.
//

#include <type_traits>
#include <cassert>
#include <vector>
#include <algorithm>

#include "tape.h"

using namespace tape;

// sort for integer sign types
template<typename T, typename Comparator = std::less<T>>
class Sort
{

    static_assert((std::is_integral_v<T> && std::is_signed_v<T>)); // TODO

    void sort(const Tape<T>& in, Tape<T>& out, const size_t& M) {
        TapeBinary<T> current("../tmp/current.bin");
        std::vector<T> block;
        size_t k = M;
        while(!in.is_eot()) {
            for (size_t i = 0; i < M && !in.is_eot(); ++i) {
                block.emplace_back(in.read());
                in.left();
            }
            std::sort(block.begin(), block.end(), Comparator());
            for (T & elem : block) {
                current.write(elem);
                current.left();
            }
        }
    }

private:

    void move_to_start(Tape<T> & tp) {
        while(tp.position() != 0) {
            tp.right();
        }
    }
};
