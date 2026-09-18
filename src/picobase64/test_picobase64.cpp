#include "../Base64SurveyRegistry.hpp"
#include <gtest/gtest.h>

// This revision of picobase64 uses GCC's prefetch builtin directly. Prefetching
// is only a performance hint, so omit it when compiling with native MSVC.
#if defined(_MSC_VER) && !defined(__clang__)
#define __builtin_prefetch(address, rw, locality) ((void)0)
#define BASE64_SURVEY_UNDEFINE_BUILTIN_PREFETCH
#endif
#include "../libs/picobase64/picobase64.h"
#ifdef BASE64_SURVEY_UNDEFINE_BUILTIN_PREFETCH
#undef BASE64_SURVEY_UNDEFINE_BUILTIN_PREFETCH
#undef __builtin_prefetch
#endif


struct picobase64
{
	static size_t decode_into(const std::string& base64, unsigned char* output)
	{
		return DecodeChunk(base64.data(), base64.size(), output);
	}

	std::string encode(const std::string& bytes)
	{
		return b64encode(bytes);
	}

	std::string decode(const std::string& base64)
	{
		return b64decode(base64);
	}
};

BASE64_REGISTER_ENCODER(picobase64);
BASE64_REGISTER_DECODER(picobase64);
BASE64_REGISTER_RAW_DECODER(picobase64);
