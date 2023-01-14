#include <iostream>
#include <immintrin.h>
#include <chrono>
#include <iostream>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <string>

namespace bits
{

    template <typename T>
    T clear_leftmost_set(const T value)
    {

        assert(value != 0);

        return value & (value - 1);
    }

    template <typename T>
    unsigned get_first_bit_set(const T value)
    {

        assert(value != 0);

        return __builtin_ctz(value);
    }

    template <>
    unsigned get_first_bit_set<uint64_t>(const uint64_t value)
    {

        assert(value != 0);

        return __builtin_ctzl(value);
    }

} // namespace bits

size_t avx2_strstr_anysize(const char *s, size_t n, const char *needle, size_t k)
{

    const __m256i first = _mm256_set1_epi8(needle[0]);
    const __m256i last = _mm256_set1_epi8(needle[k - 1]);

    for (size_t i = 0; i < n; i += 32)
    {

        const __m256i block_first = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(s + i));
        const __m256i block_last = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(s + i + k - 1));

        const __m256i eq_first = _mm256_cmpeq_epi8(first, block_first);
        const __m256i eq_last = _mm256_cmpeq_epi8(last, block_last);

        uint32_t mask = _mm256_movemask_epi8(_mm256_and_si256(eq_first, eq_last));

        while (mask != 0)
        {

            const auto bitpos = bits::get_first_bit_set(mask);

            if (memcmp(s + i + bitpos + 1, needle + 1, k - 2) == 0)
            {
                return i + bitpos;
            }

            mask = bits::clear_leftmost_set(mask);
        }
    }

    return std::string::npos;
}

// g++ simd.cpp -mavx2 -O2
/*
rm -f out.perf && rm -f perf.data && rm -f test.svg && rm -f out.folded && rm -f test.svg
perf record -F 99 --call-graph dwarf ./a.out
perf script > out.perf && ./FlameGraph/stackcollapse-perf.pl out.perf > out.folded && ./FlameGraph/flamegraph.pl out.folded > test.svg
*/

int n;

int main()
{
    std::string str("abc----------------------------\r\n");
    auto t1 = std::chrono::high_resolution_clock::now();
    const char crlf[] = {'\r', '\n'};
    auto searcher = std::boyer_moore_horspool_searcher(crlf, crlf + 2);
    auto *start = str.data();
    for (size_t i = 0; i < 1'000'000'000; ++i)
    {
        // auto pos = avx2_strstr_anysize(str.c_str(), str.size(), "\r\n", 2);
        // n = pos;
        auto *ptr = strstr(str.c_str(), "\r\n");
        n = static_cast<int>(*ptr);

        // auto *it = std::search(
        //     str.c_str(), str.c_str() + str.size(), searcher);
        // n = it - start;

        // auto pos = str.find("\r\n");
        // n = pos;
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto time_1 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "---" << time_1.count() << '\n';

    auto t3 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 1'000'000'000; ++i)
    {
        // auto pos = str.find("\r\n");
        auto pos = avx2_strstr_anysize(str.c_str(), str.size(), "\r\n", 2);
        n = pos;
    }
    auto t4 = std::chrono::high_resolution_clock::now();
    auto time_2 = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3);
    std::cout << "---" << time_2.count() << '\n';
}
