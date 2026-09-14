#include "Base64SurveyRegistry.hpp"

namespace b64_survey {
using namespace std;

map<string, Base64EncoderProc>& Base64SurveyRegistry::GetEncoders()
{
    static map<string, Base64EncoderProc> s_tests;
    return s_tests;
}

map<string, Base64DecoderProc>& Base64SurveyRegistry::GetDecoders()
{
    static map<string, Base64DecoderProc> s_tests;
    return s_tests;
}

map<string, Base64RawEncoderProc>& Base64SurveyRegistry::GetRawEncoders()
{
    static map<string, Base64RawEncoderProc> s_tests;
    return s_tests;
}

map<string, Base64RawDecoderProc>& Base64SurveyRegistry::GetRawDecoders()
{
    static map<string, Base64RawDecoderProc> s_tests;
    return s_tests;
}

} // namespace b64_survey
