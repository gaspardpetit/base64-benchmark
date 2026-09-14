#pragma once

#include "Base64SurveyRegistry.hpp"
#include "Base64SurveyReport.hpp"

#include <algorithm>
#include <vector>
#include <chrono>
#include <cstring>

namespace b64_survey {
using namespace std;
using namespace std::chrono;

struct Base64SurveyBenchmark
{
    template<typename _FN>
    static pair<int, microseconds> RunUntil(int maxIter, chrono::milliseconds maxMS, _FN &&fn)
    {
        // Grow batches until they take about 1 ms, amortizing clock reads
        // while keeping the time-limit overshoot small for slow conversions.
        using Clock = chrono::steady_clock;
        const auto t1 = Clock::now();
        auto batchStart = t1;
        auto t2 = t1;
        constexpr int maxBatchSize = 65536;
        int batchSize = 1;
        int iter = 0;
        while (iter < maxIter)
        {
            const int count = std::min(batchSize, maxIter - iter);
            for (int i = 0; i < count; ++i)
                fn();
            iter += count;

            t2 = Clock::now();
            if (t2 - t1 >= maxMS)
                break;

            if (t2 - batchStart < chrono::milliseconds(1))
                batchSize = std::min(batchSize * 2, maxBatchSize);
            batchStart = t2;
        }

        const auto elapsed = duration_cast<chrono::microseconds>(t2 - t1);
        return make_pair(iter, elapsed);
    }

    template<class _ProgressTy>
    static BenchmarkResults RunEncodeBenchmark(const vector<int> &testCases, _ProgressTy &progress, int maxIter, milliseconds maxMS)
    {
        BenchmarkResults results;
        vector<string> buffers{ testCases.size() };
        vector<string> value_buffers{ testCases.size() };
        for (int i = 0; i < testCases.size(); ++i)
        {
            buffers[i].resize(testCases[i]);
            for (int j = 0; j < testCases[i]; ++j)
                buffers[i][j] = rand();
        }

        for (int i = 0; i < testCases.size(); ++i)
        {
            value_buffers[i] = Base64SurveyRegistry::GetEncoders().begin()->second(buffers[i]);
        }

        for (const pair<string, Base64EncoderProc> &entry : Base64SurveyRegistry::GetEncoders())
        {
            const string &name = entry.first;
            progress.onBeginSuite(name);

            for (int testN = 0; testN < testCases.size(); ++testN)
            {
                pair<int, microseconds> result = RunUntil(maxIter, maxMS, [&]() {
                    (entry.second)(buffers[testN]);
                    });
                double time_tiny = double(result.second.count()) / double(result.first);
                results[name][testCases[testN]] = time_tiny;
                progress.onCaseResult(testN, time_tiny);
            }
        }

        progress.onFinish();
        return results;
    }

    template<class _ProgressTy>
    static BenchmarkResults RunRawEncodeBenchmark(const vector<int> &testCases,
        _ProgressTy &progress, int maxIter, milliseconds maxMS)
    {
        BenchmarkResults results;
        vector<string> inputs(testCases.size());
        vector<string> expected(testCases.size());
        vector<vector<char>> outputs(testCases.size());
        for (size_t i = 0; i < testCases.size(); ++i) {
            inputs[i].resize(testCases[i]);
            for (size_t j = 0; j < inputs[i].size(); ++j)
                inputs[i][j] = static_cast<char>(rand());
            expected[i] = Base64SurveyRegistry::GetEncoders().begin()->second(
                inputs[i]);
            outputs[i].resize(expected[i].size() + 16U);
        }

        for (const auto &entry : Base64SurveyRegistry::GetRawEncoders()) {
            progress.onBeginSuite(entry.first);
            for (size_t i = 0; i < testCases.size(); ++i) {
                const size_t size = entry.second(inputs[i], outputs[i].data());
                if (size != expected[i].size() ||
                    std::memcmp(expected[i].data(), outputs[i].data(), size) != 0) {
                    cout << "failure in raw encoder " << entry.first << endl;
                    break;
                }
                const auto result = RunUntil(maxIter, maxMS, [&]() {
                    entry.second(inputs[i], outputs[i].data());
                });
                const double time = double(result.second.count()) /
                                    double(result.first);
                results[entry.first][testCases[i]] = time;
                progress.onCaseResult(static_cast<int>(i), time);
            }
        }
        progress.onFinish();
        return results;
    }

    template<class _ProgressTy>
    static BenchmarkResults RunDecodeBenchmark(const std::vector<int> &testCases, _ProgressTy &progress, int maxIter, milliseconds maxMS)
    {
        BenchmarkResults results;
        std::vector<string> buffers{ testCases.size() };
        std::vector<string> value_buffers{ testCases.size() };
        for (int i = 0; i < testCases.size(); ++i)
        {
            buffers[i].resize(testCases[i]);
            for (int j = 0; j < testCases[i]; ++j)
                buffers[i][j] = rand();
        }

        for (int i = 0; i < testCases.size(); ++i)
        {
            value_buffers[i] = Base64SurveyRegistry::GetEncoders().begin()->second(buffers[i]);
        }

        for (const pair<string, Base64DecoderProc> &entry : Base64SurveyRegistry::GetDecoders())
        {
            const string &name = entry.first;
            progress.onBeginSuite(name);

            for (int testN = 0; testN < testCases.size(); ++testN)
            {
                std::string decodeResult = (entry.second)(value_buffers[testN]);
                if (decodeResult != buffers[testN])
                {
                    cout << "failure, expected " << buffers[testN] << " but got " << decodeResult
                        << endl;
                    break;
                }

                pair<int, microseconds> result = RunUntil(maxIter, maxMS, [&]() {
                    (entry.second)(value_buffers[testN]);
                    });

                double time_tiny = double(result.second.count()) / double(result.first);
                results[name][testCases[testN]] = time_tiny;
                progress.onCaseResult(testN, time_tiny);
            }
        }

        progress.onFinish();
        return results;
    }

    template<class _ProgressTy>
    static BenchmarkResults RunRawDecodeBenchmark(const std::vector<int> &testCases,
        _ProgressTy &progress, int maxIter, milliseconds maxMS)
    {
        BenchmarkResults results;
        std::vector<string> inputs(testCases.size());
        std::vector<string> encoded(testCases.size());
        std::vector<std::vector<unsigned char>> outputs(testCases.size());
        for (size_t i = 0; i < testCases.size(); ++i) {
            inputs[i].resize(testCases[i]);
            for (size_t j = 0; j < inputs[i].size(); ++j)
                inputs[i][j] = static_cast<char>(rand());
            encoded[i] = Base64SurveyRegistry::GetEncoders().begin()->second(inputs[i]);
            outputs[i].resize(inputs[i].size() + 16U);
        }

        for (const auto &entry : Base64SurveyRegistry::GetRawDecoders()) {
            progress.onBeginSuite(entry.first);
            for (size_t i = 0; i < testCases.size(); ++i) {
                const size_t size = entry.second(encoded[i], outputs[i].data());
                if (size != inputs[i].size() ||
                    std::memcmp(inputs[i].data(), outputs[i].data(), size) != 0) {
                    cout << "failure in raw decoder " << entry.first << endl;
                    break;
                }
                const auto result = RunUntil(maxIter, maxMS, [&]() {
                    entry.second(encoded[i], outputs[i].data());
                });
                const double time = double(result.second.count()) / double(result.first);
                results[entry.first][testCases[i]] = time;
                progress.onCaseResult(static_cast<int>(i), time);
            }
        }
        progress.onFinish();
        return results;
    }
};

} // namespace b64_survey
