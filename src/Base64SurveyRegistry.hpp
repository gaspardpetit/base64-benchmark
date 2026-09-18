#pragma once

#include "Base64SurveyTests.hpp"
#include <string>
#include <map>
#include <gtest/gtest.h>

namespace b64_survey {
using namespace std;

using Base64EncoderProc = string(*)(const string &);
using Base64RawEncoderProc = size_t(*)(const string &, char *);
using Base64DecoderProc = string(*)(const string &);
using Base64RawDecoderProc = size_t(*)(const string &, unsigned char *);

struct Base64SurveyRegistry
{
	static map<string, Base64EncoderProc>& GetEncoders();
	static map<string, Base64RawEncoderProc>& GetRawEncoders();
	static map<string, Base64DecoderProc>& GetDecoders();
	static map<string, Base64RawDecoderProc>& GetRawDecoders();

	template<class BASE64Impl>
	struct RegisterEncodeTest
	{
		explicit RegisterEncodeTest(const string &name)
		{
			GetEncoders()[name] = [](const string &bytes) {
				BASE64Impl impl;
				return impl.encode(bytes);
			};
		}
	};

	template<class BASE64Impl>
	struct RegisterRawEncodeTest
	{
		explicit RegisterRawEncodeTest(const string &name)
		{
			GetRawEncoders()[name] = [](const string &bytes, char *output) {
				return BASE64Impl::encode_into(bytes, output);
			};
		}
	};

	template<class BASE64Impl>
	struct RegisterRawDecodeTest
	{
		explicit RegisterRawDecodeTest(const string &name)
		{
			GetRawDecoders()[name] = [](const string &bytes,
			                                  unsigned char *output) {
				return BASE64Impl::decode_into(bytes, output);
			};
		}
	};

	template<class BASE64Impl>
	struct RegisterDecodeTest
	{
		explicit RegisterDecodeTest(const string &name)
		{
			GetDecoders()[name] = [](const string &bytes) {
				BASE64Impl impl;
				return impl.decode(bytes);
			};
		}
	};
};

#define BASE64_REGISTER_ENCODER_NAMED(TEST_TYPE, DISPLAY_NAME)\
b64_survey::Base64SurveyRegistry::RegisterEncodeTest<TEST_TYPE> register_##TEST_TYPE##_encode(DISPLAY_NAME);\
BASE64_REGISTER_ENCODER_TESTS(TEST_TYPE)

#define BASE64_REGISTER_ENCODER(TEST_TYPE)\
BASE64_REGISTER_ENCODER_NAMED(TEST_TYPE, #TEST_TYPE)

#define BASE64_REGISTER_DECODER_NAMED(TEST_TYPE, DISPLAY_NAME)\
b64_survey::Base64SurveyRegistry::RegisterDecodeTest<TEST_TYPE> register_##TEST_TYPE##_decode(DISPLAY_NAME);\
BASE64_REGISTER_DECODER_TESTS(TEST_TYPE)

#define BASE64_REGISTER_DECODER(TEST_TYPE)\
BASE64_REGISTER_DECODER_NAMED(TEST_TYPE, #TEST_TYPE)

#define BASE64_REGISTER_RAW_ENCODER_NAMED(TEST_TYPE, DISPLAY_NAME)\
b64_survey::Base64SurveyRegistry::RegisterRawEncodeTest<TEST_TYPE> register_##TEST_TYPE##_raw_encode(DISPLAY_NAME)

#define BASE64_REGISTER_RAW_ENCODER(TEST_TYPE)\
BASE64_REGISTER_RAW_ENCODER_NAMED(TEST_TYPE, #TEST_TYPE)

#define BASE64_REGISTER_RAW_DECODER_NAMED(TEST_TYPE, DISPLAY_NAME)\
b64_survey::Base64SurveyRegistry::RegisterRawDecodeTest<TEST_TYPE> register_##TEST_TYPE##_raw_decode(DISPLAY_NAME)

#define BASE64_REGISTER_RAW_DECODER(TEST_TYPE)\
BASE64_REGISTER_RAW_DECODER_NAMED(TEST_TYPE, #TEST_TYPE)

} // namespace b64_survey
