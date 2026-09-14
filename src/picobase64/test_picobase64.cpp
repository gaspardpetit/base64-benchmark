#include "../Base64SurveyRegistry.hpp"
#include <gtest/gtest.h>
#include "../libs/picobase64/picobase64.h"


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
