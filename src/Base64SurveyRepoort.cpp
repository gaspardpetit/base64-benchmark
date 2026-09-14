#include "Base64SurveyReport.hpp"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <cctype>

namespace b64_survey {
using namespace std;

string Base64SurveyReport::GetCompilerSuffix()
{
#if defined(_MSC_VER)
    int ver = _MSC_VER;
    stringstream strm;
    strm << "msvc" << ver;
    return strm.str();
#elif defined(__clang_major__)
    int ver = __clang_major__;
    stringstream strm;
    strm << "clang" << ver;
    return strm.str();
#else
    return "unknown";
#endif
}


string Base64SurveyReport::DumpJsonResult(const string &name, BenchmarkResults &results)
{
    // create result directory
    static const std::string resultDir = "result/";

    string path = resultDir + name + "-" + Base64SurveyReport::GetCompilerSuffix() + ".js";
    string varName = name + "_result_" + Base64SurveyReport::GetCompilerSuffix();
    for (char &character : varName)
        if (!std::isalnum(static_cast<unsigned char>(character)))
            character = '_';
    filesystem::create_directory(resultDir);

    ofstream outFile(path);
    if (outFile)
    {
        //
        outFile << "var " << varName << " = [\n";
        //
        uint32_t cnt = 0;
        for (auto i : results)
        {
            // get
            auto [name, statVal] = i;
            if (cnt > 0)
                outFile << ",\n";

            cnt++;
            outFile << "\t{name:'" << name << "',\n";
            outFile << "\tvalues:[\n";

            uint32_t rcnt = 0;
            for (auto j : statVal)
            {
                auto [len, t] = j;
                if (rcnt > 0)
                    outFile << ",\n";
                rcnt++;
                outFile << "\t\t{len:" << len << ",time:" << t << "}";
            }
            outFile << "\n\t]}";
        }
        outFile << "]\n";
        outFile.close();
    }
    else
    {
        cerr << "can not create file " << path << "\n";
    }

    return path;
}

} // namespace b64_survey
