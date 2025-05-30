#include <benchmark/benchmark.h>

#include <array>
#include <string>
#include <vector>

#include <boost/container/small_vector.hpp>

#include <llvm/ADT/SmallVector.h>

#include "vector.h"

template<typename T>
static void main_test(benchmark::State &state) {
    static constexpr int N = 400;

    for (auto _: state) {
        std::array<T, N> a;

        for (int i = 0; i < N; i++) {
            auto &v = a[i];

            for (int j = 0; j < 8; j++)
                v.push_back("Hello!");

            benchmark::DoNotOptimize(v.data());
            benchmark::ClobberMemory();
        }

        benchmark::DoNotOptimize(a.data());
        benchmark::ClobberMemory();
    }
}

using value_type = std::string;

BENCHMARK_TEMPLATE(main_test, std::vector<value_type>);
BENCHMARK_TEMPLATE(main_test, test::vector<value_type, 8>);
BENCHMARK_TEMPLATE(main_test, boost::container::small_vector<value_type, 8>);
BENCHMARK_TEMPLATE(main_test, llvm::SmallVector<value_type, 8>);

BENCHMARK_MAIN();
