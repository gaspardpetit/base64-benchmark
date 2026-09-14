# Base64 C/C++ Benchmarks

This repository tests and compares the correctness and performance of C and C++ Base64 encoding and decoding implementations across input sizes, compilers, and platforms.

## Benchmark results

**[View the latest benchmark results](https://gaspardpetit.github.io/base64-benchmark/)**

## Conclusion

- SIMD-accelerated implementations provide the highest sustained throughput on large payloads. In the Windows 32 KiB combined result, [TurboBase64](https://github.com/powturbo/Turbo-Base64) reaches 10.799 GB/s, followed by [base64 (Alfred Klomp)](https://github.com/aklomp/base64) at 8.803 GB/s and [simdutf](https://github.com/simdutf/simdutf) at 7.173 GB/s.
- [GaspardPetit](https://github.com/gaspardpetit/base64) is the leading architecture-neutral implementation in the current Windows ranking at 2.680 GB/s. It also leads the architecture-neutral encoders and decoders at 32 KiB in the published Linux and macOS results.
- [base64 (Tobias Locker)](https://github.com/tobiaslocker/base64) and [Chromium's `modp_b64`](https://chromium.googlesource.com/chromium/src/third_party/modp_b64/) remain close to each other, as expected from their related designs. At 32 KiB on Windows their combined throughput is 1.999 GB/s and 1.936 GB/s, respectively.
- For small payloads, fixed call and wrapper costs are a significant part of the measurement. Differences at 32 bytes are often only a few nanoseconds and should not be treated as meaningful without repeated measurements on the target system.
- Rankings change with the compiler, architecture, payload size, and operation. Encoding and decoding results should therefore be considered separately when only one direction matters.
- Performance is only one selection criterion. Licensing, malformed-input handling, streaming support, memory allocation, API design, and portability differ among these implementations and are not normalized by this benchmark.

Results from different platform tabs were collected on different machines and should not be used for direct cross-platform hardware comparisons. Benchmark the relevant candidates in the target application before choosing an implementation.

Contributions that improve the benchmark or add implementations are welcome.

## Build and run

After cloning the repository, initialize its dependencies from the project root:

```sh
git submodule update --init --recursive
```

### Windows

Install Visual Studio 2026 with the **Desktop development with C++** workload, the MSVC v143 toolset targeted by the checked-in projects, and a Windows SDK.

1. Open `build/msvc/base64.sln` in Visual Studio.
2. Select the **Release** configuration and **x64** platform.
3. Build the solution.
4. Set `base64` as the startup project and choose **Debug > Start Without Debugging** (`Ctrl+F5`) to run it.

### Linux and macOS

Install CMake 3.25 or newer, a C/C++ compiler with C++20 support, and Make. On macOS, the compiler and Make are available through the Xcode Command Line Tools (`xcode-select --install`); install CMake separately.

The configure, build, and run commands are identical on Linux and macOS. Run these from the project root:

```sh
cmake -S . -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/base64-benchmark
```

CMake fetches GoogleTest during configuration, so the initial configuration requires internet access.

### Running the benchmarks

The executable first runs correctness tests, then benchmarks encoding and decoding for input sizes from 32 bytes to 64 KiB. It prints results and writes `encode-*.js` and `decode-*.js` reports to a `result/` directory relative to its working directory.

Use `--raw-only` to benchmark implementations that can write directly into reusable caller-provided storage. This mode validates each encoder and decoder before timing, allocates output buffers once, and does not clear or resize them inside the timed loop. It writes `encode-raw-*.js` and `decode-raw-*.js` reports.

For a CMake build, you can also run it through CTest:

```sh
ctest --test-dir build -C Release --output-on-failure
```

CTest runs the full benchmarks as well as the correctness tests, and writes reports under `build/result/`.

Timings are average microseconds per conversion, including the implementation wrapper. Each case runs up to one million conversions or approximately 500 ms. The benchmark uses a monotonic clock and checks elapsed time between adaptive batches, doubling the batch size while batches take less than 1 ms, up to 65,536 conversions per batch. This amortizes clock-reading overhead; the time limit can be exceeded by the final batch. Earlier reports used a clock check after every conversion, so small-input timings are not directly comparable across the two measurement methods.

### Disclaimer

Some implementations were modified to use a common benchmark interface. Line wrapping is disabled for both encoding and decoding. The correctness tests cover valid inputs; malformed-input handling is not compared and may account for some performance differences.

The license identifiers below are provided as a convenience and may not account for every file or dependency. Verify the upstream license before using an implementation.

## Compared implementations

Ranked by combined encoding and decoding time for a 32 KiB payload in the published Windows results. Throughput is the original 32 KiB payload size divided by the total encoding and decoding time, using decimal GB/s. Lower ranks are faster.

| Rank | 32 KiB throughput | Implementation | Source | License | Notes |
| ---: | ---: | --- | --- | --- | --- |
| 1 | 10.799 GB/s | Turbob64 | [powturbo/Turbo-Base64](https://github.com/powturbo/Turbo-Base64) | GPL-3.0 | SIMD-accelerated implementation. |
| 2 | 8.803 GB/s | base64 (Alfred Klomp) | [aklomp/base64](https://github.com/aklomp/base64) | BSD-2-Clause | C99 implementation with SIMD acceleration. |
| 3 | 7.173 GB/s | simdutf | [simdutf/simdutf](https://github.com/simdutf/simdutf) | Apache-2.0 OR MIT | C++ Unicode and Base64 library with SIMD acceleration. |
| 4 | 2.680 GB/s | GaspardPetit | [gaspardpetit/base64](https://github.com/gaspardpetit/base64) | BSD-3-Clause | Header-only and compiled C/C++ implementation. |
| 5 | 1.999 GB/s | base64 (Tobias Locker) | [tobiaslocker/base64](https://github.com/tobiaslocker/base64) | MIT | Single-header C++ implementation based on Nick Galbreath's `modp_b64` approach. |
| 6 | 1.936 GB/s | ModpB64Chromium | [Chromium `modp_b64`](https://chromium.googlesource.com/chromium/src/third_party/modp_b64/) | BSD-3-Clause | Chromium's copy of `modp_b64`. |
| 7 | 1.585 GB/s | Adition | [Benchmark source](src/adition) | Unknown | Implementation contributed directly to this repository. |
| 8 | 1.569 GB/s | Polfosol_IMUtility | [IMProject/IMUtility](https://github.com/IMProject/IMUtility) | BSD-3-Clause | IMUtility's adaptation of the Polfosol implementation. |
| 9 | 1.554 GB/s | picobase64 | [GermanAizek/picobase64](https://github.com/GermanAizek/picobase64) | GPL-3.0 | Header-only C++ implementation. |
| 10 | 1.336 GB/s | libcurl | [curl](https://curl.se/libcurl/) | curl | — |
| 11 | 1.308 GB/s | NibbleAndAHalf | [superwills/NibbleAndAHalf](https://github.com/superwills/NibbleAndAHalf/) | Zlib | — |
| 12 | 1.055 GB/s | polfosol | [Stack Overflow](https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c) | Unknown | — |
| 13 | 0.911 GB/s | Gnome | [GNOME GLib](https://github.com/GNOME/glib/blob/main/glib/gbase64.c) | LGPL-2.0-or-later | — |
| 14 | 0.847 GB/s | apache | [Apple Open Source](https://github.com/apple-oss-distributions/apache1/blob/apache1-697/apache1/src/ap/ap_base64.c) | Apache-1.1 | Believed to be based on Rob McCool's 1993 `uuencode` implementation. |
| 15 | 0.751 GB/s | cppcodec | [tplgy/cppcodec](https://github.com/tplgy/cppcodec) | MIT | — |
| 16 | 0.685 GB/s | test_wikibooks_org_c | [Wikibooks](https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64) | CC BY-SA | C implementation. |
| 17 | 0.665 GB/s | TomyKaria | [tomykaira gist](https://gist.github.com/tomykaira/f0fd86b6c73063283afe550bc5d77594) | MIT | Single-header C++ implementation. |
| 18 | 0.629 GB/s | jounimalinen | [FreeBSD WPA utilities](https://web.mit.edu/freebsd/head/contrib/wpa/src/utils/base64.c) | BSD | — |
| 19 | 0.562 GB/s | libb64 | [SourceForge](https://sourceforge.net/projects/libb64/) | Public domain | — |
| 20 | 0.387 GB/s | Manuel Martinez | [Stack Overflow](https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c) | Unknown | — |
| 21 | 0.359 GB/s | base64 (Mat Gomes) | [matheusgomes28/base64pp](https://github.com/matheusgomes28/base64pp) | MIT | C++ implementation accompanied by a [technical article](https://matgomes.com/base64-encode-decode-cpp/). |
| 22 | 0.324 GB/s | DaedalusAlpha | [Stack Overflow](https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c) | Unknown | — |
| 23 | 0.272 GB/s | Elegant Dice | [Stack Overflow](https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c) | Unknown | — |
| 24 | 0.154 GB/s | Boost | [Boost.Serialization](https://github.com/boostorg/serialization) | BSL-1.0 | Uses the Base64 archive iterators. |
| 25 | 0.144 GB/s | Arduino-Base64 | [adamvr/arduino-base64](https://github.com/adamvr/arduino-base64) | MIT | Uses [BuLogics/libb64](https://github.com/BuLogics/libb64). |
| 26 | 0.140 GB/s | test_wikibooks_org_cpp | [Wikibooks](https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64) | CC BY-SA | C++ implementation. |
| 27 | 0.120 GB/s | adp-gmbh (René Nyffenegger) | [adp-gmbh.ch](http://www.adp-gmbh.ch/cpp/common/base64.html) | Zlib | — |
| 28 | 0.091 GB/s | Internet Software Consortium | [Apple Open Source](https://github.com/apple-oss-distributions/basic_cmds/blob/basic_cmds-55/uuencode/base64.c) | ISC and IBM notices | — |
| 29 | 0.065 GB/s | LihO | [Stack Overflow](https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c) | Unknown | — |
| 30 | 0.045 GB/s | Embedded Template Library (ETL) | [ETLCPP/etl](https://github.com/ETLCPP/etl) | MIT | Streaming, fixed-capacity C++ implementation intended for embedded systems. |
| 31 | 0.012 GB/s | user152949 | [Stack Overflow](https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c) | Unknown | — |
| — | — | A.Hristov | [Benchmark source](src/A.Hristov) | Unknown | Not present in the published Windows results. |
| — | — | omnifarious | [Stack Overflow](https://stackoverflow.com/questions/5288076/base64-encoding-and-decoding-with-openssl) | Unknown | Does not always produce valid output; not present in the published Windows results. |
| — | — | Polfosol_mod | [Stack Overflow](https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c/13935718) | Unknown | Decoder-only variant. |
