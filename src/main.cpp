//=============================================================================
// EXTERNAL DEPENDENCIES
//=============================================================================
#include "Base64SurveyRegistry.hpp"
#include "Base64SurveyReport.hpp"
#include "Base64SurveyBenchmark.hpp"

#include <iostream>
#include <filesystem>
#include <string_view>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

//=============================================================================
// NAMESPACE IMPORTS
//=============================================================================
using namespace std::chrono;
using namespace std;
using namespace b64_survey;

static const std::vector<int> test_sizes{ 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536 };

int main(int argc, char *argv[])
{
    bool testsOnly = false;
    bool rawOnly = false;
    for (int i = 1; i < argc; ++i) {
        testsOnly |= std::string_view(argv[i]) == "--tests-only";
        rawOnly |= std::string_view(argv[i]) == "--raw-only";
    }

#ifdef _WIN32
    // try to get more consistent results
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
#endif // _WIN32

    // Run smoke tests on each implementation
    int result = EXIT_SUCCESS;
    if (!rawOnly) {
        result = Base64SurveyTests::RunTests(argc + 1, argv);
        //cout << "Press enter to continue." << endl;
        //getchar();
    }

    if (testsOnly)
        return result;

    // Profile the encoders
    if (!rawOnly) {
        cout << endl;
        cout << "*** PROFILING ENCODERS ***" << endl;
        cout << endl;
        MarkdownResultsDelegate liveProgress{ test_sizes };
        BenchmarkResults encodeResults = Base64SurveyBenchmark::RunEncodeBenchmark(test_sizes, liveProgress, 1000000, 500ms);

        cout << endl;
        cout << "*** SORTING ENCODER RESULTS ***" << endl;
        cout << endl;
        MarkdownResultsDelegate sortedProgress{ test_sizes };
        Base64SurveyReport::PrintSortedResults(encodeResults, sortedProgress);

        // create stat javascript files
        string savedPath = Base64SurveyReport::DumpJsonResult("encode", encodeResults);
        cout << "Saved encoder report to " + savedPath;
    }

    cout << endl;

    // Profile encoders that can write into reusable caller-provided storage.
    if (rawOnly) {
        cout << endl << "*** PROFILING RAW ENCODERS ***" << endl << endl;
        MarkdownResultsDelegate liveProgress{ test_sizes };
        BenchmarkResults rawEncodeResults =
            Base64SurveyBenchmark::RunRawEncodeBenchmark(
                test_sizes, liveProgress, 1000000, 500ms);

        cout << endl << "*** SORTING RAW ENCODER RESULTS ***" << endl << endl;
        MarkdownResultsDelegate sortedProgress{ test_sizes };
        Base64SurveyReport::PrintSortedResults(rawEncodeResults,
                                               sortedProgress);

        std::string savedPath = Base64SurveyReport::DumpJsonResult(
            "encode-raw", rawEncodeResults);
        cout << "Saved raw encoder report to " + savedPath;
    }

    cout << endl;

    // Profile decoders that can write into reusable caller-provided storage.
    if (rawOnly) {
        cout << endl << "*** PROFILING RAW DECODERS ***" << endl << endl;
        MarkdownResultsDelegate liveProgress{ test_sizes };
        BenchmarkResults rawDecodeResults = Base64SurveyBenchmark::RunRawDecodeBenchmark(
            test_sizes, liveProgress, 1000000, 500ms);

        cout << endl << "*** SORTING RAW DECODER RESULTS ***" << endl << endl;
        MarkdownResultsDelegate sortedProgress{ test_sizes };
        Base64SurveyReport::PrintSortedResults(rawDecodeResults, sortedProgress);

        std::string savedPath = Base64SurveyReport::DumpJsonResult(
            "decode-raw", rawDecodeResults);
        cout << "Saved raw decoder report to " + savedPath;
    }

    cout << endl;

    if (rawOnly)
        return result;

    // Profile the decoders
    if (!rawOnly) {
        cout << endl;
        cout << "*** PROFILING DECODERS ***" << endl;
        cout << endl;
        MarkdownResultsDelegate liveProgress{ test_sizes };
        BenchmarkResults decodeResults = Base64SurveyBenchmark::RunDecodeBenchmark(test_sizes, liveProgress, 1000000, 500ms);

        cout << endl;
        cout << "*** SORTING DECODER RESULTS ***" << endl;
        cout << endl;
        MarkdownResultsDelegate sortedProgress{ test_sizes };
        Base64SurveyReport::PrintSortedResults(decodeResults, sortedProgress);

        // create stat javascript files
        std::string savedPath = Base64SurveyReport::DumpJsonResult("decode", decodeResults);
        cout << "Saved decoder report to " + savedPath;
    }

    //cout << "Press enter to continue." << endl;
    //getchar();
    return result;
}
