# customVectorImpl

## Test Results:

### Measurement Results (Clang)

| Library                          | Time (ns) |
|----------------------------------|-----------|
| `std::vector`                    | 46988     |
| `test::vector`                   | 24420     |
| `boost::container::small_vector` | 21153     |
| `llvm::SmallVector`              | 23448     |

### Measurement Results (GCC)

| Library                          | Time (ns) |
|----------------------------------|-----------|
| `std::vector`                    | 50664     |
| `test::vector`                   | 25235     |
| `boost::container::small_vector` | 20536     |
| `llvm::SmallVector`              | 24515     |
