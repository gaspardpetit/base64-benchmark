# Benchmark results Results for Encoding


[The latest results are here](https://gaspardpetit.github.io/base64-benchmark/)

# base64

Searching the web for a C/C++ base64 implementation yields many results.
 
 
How does one chose?
This project aims at collecting and comparing the different implementations available.
 
Feel free to improve this test or to submit new implementations! 

## Build and run

After cloning the repository, initialize its dependencies from the project root:

```sh
git submodule update --init --recursive
```

### Windows

Install Visual Studio 2022 with the **Desktop development with C++** workload, including the MSVC v143 toolset and Windows SDK.

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

For a CMake build, you can also run it through CTest:

```sh
ctest --test-dir build -C Release --output-on-failure
```

CTest runs the full benchmarks as well as the correctness tests, and writes reports under `build/result/`.

Timings are average microseconds per conversion, including the implementation wrapper. Each case runs up to one million conversions or approximately 500 ms. The benchmark uses a monotonic clock and checks elapsed time between adaptive batches, doubling the batch size while batches take less than 1 ms, up to 65,536 conversions per batch. This amortizes clock-reading overhead; the time limit can be exceeded by the final batch. Earlier reports used a clock check after every conversion, so small-input timings are not directly comparable across the two measurement methods.

### Disclaimer

Keep in mind that some implementations were slightly modified to have a common interface for the test framework.  In all cases, line breaks were omitted both for encoding and parsing. Also, although all implementations have been unit-tested, no analysis whatsoever has been made on error handling, so some slower implementation may actually be better at reporting malformed base64 strings for instance.

I do not own any of these implementations - make sure you check their respective license before using them.

## Compared implementations

### Embedded Template Library (ETL)
From https://github.com/ETLCPP/etl
A streaming, fixed-capacity C++ implementation intended for embedded systems

### base64 (Alfred Klomp)
From https://github.com/aklomp/base64
A fast C99 implementation with SIMD acceleration 

### base64 (Tobias Locker)
From https://github.com/tobiaslocker/base64
A c++ single-header library implementing the approach of Nick Galbreath's in  `modp_b64`

### simdutf
From https://github.com/simdutf/simdutf
A unicode validation and transcoding c++ library accelerated using SIMD instructions

### base64 (Mat Gomes)
From https://github.com/matheusgomes28/base64pp
A c++ library written for readibility and supported by a blog post https://matgomes.com/base64-encode-decode-cpp/

### adp-gmbh (René Nyffenegger)
From http://www.adp-gmbh.ch/cpp/common/base64.html

### lib64
From https://sourceforge.net/projects/libb64/
 
### libcurl
From https://curl.haxx.se/libcurl/
 
### Internet Software Consortium
From https://opensource.apple.com/source/basic_cmds/basic_cmds-55/uuencode/base64.c
 
### Arduino-Base64
From https://github.com/adamvr/arduino-base64 from BuLogic's fork (https://github.com/BuLogics/libb64)
 
### LihO
From stackoverflow thread at http://stackoverflow.com/questions/180947/base64-decode-snippet-in-c

### Manuel Martinez
From stackoverflow thread at http://stackoverflow.com/questions/180947/base64-decode-snippet-in-c

### DaedalusAlpha
From stackoverflow thread at http://stackoverflow.com/questions/180947/base64-decode-snippet-in-c

### Elegant Dice
From stackoverflow thread at http://stackoverflow.com/questions/180947/base64-decode-snippet-in-c
 
### polfosol
From stackoverflow thread at http://stackoverflow.com/questions/180947/base64-decode-snippet-in-c

### omnifarious
From stackoverflow thread at http://stackoverflow.com/questions/5288076/doing-base64-encoding-and-decoding-in-openssl-c

** Note that although this implementation is in the results, it does not produce valid results - including extra '=' padding when not necessary.

### user152949
From stackoverflow thread at http://stackoverflow.com/questions/180947/base64-decode-snippet-in-c

### jounimalinen
From http://web.mit.edu/freebsd/head/contrib/wpa/src/utils/base64.c

### test_wikibooks_org_c
From https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64

### test_wikibooks_org_cpp
From https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64

### apache
Which I think was based on Rob McCool's uuencode's implementation in 1993
https://opensource.apple.com/source/QuickTimeStreamingServer/QuickTimeStreamingServer-452/CommonUtilitiesLib/base64.c

### NibbleAndAHalf
From https://github.com/superwills/NibbleAndAHalf/

### Gnome
From https://github.com/GNOME/glib/blob/master/glib/gbase64.c

### cppcodec
From https://github.com/tplgy/cppcodec

## Conclusion

- If you want the fastest implementation, [Turbo-Base64](https://github.com/powturbo/Turbo-Base64) (GPL-3.0) is the fastest. Slightly behind, and with more permissive licenses, [simdutf](https://github.com/simdutf/simdutf) (MIT License) and [base64 (Alfred Klomp)](https://github.com/aklomp/base64) (BSD-2 License) are both great alternatives.
- If you want a simple single-header c++ file with good performance, go for [base64 (Tobias Locker)](https://github.com/tvercaut/base64)
- Some other implementations are terrible, the slower ones generally allocate memory dynamically as they convert, even though the output size is deterministic and can be preallocated;
- libb64 shows very strange performance results - I get the feeling that they are doing something neat but that I am perhaps not using their code right;
- A lot of implementations are available, everyone claims theirs is better. This test shows that unless you measure, you cannot make any claims about the performance of your code;

Keep in mind that these results were all obtained using a specific compiler and could differ on another compiler.
