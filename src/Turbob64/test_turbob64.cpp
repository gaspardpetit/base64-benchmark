#include <gtest/gtest.h>
#include "../Base64SurveyRegistry.hpp"
#include "Turbo-Base64/turbob64.h"

struct B64_Adapt
{
	static constexpr size_t GetEncodedLengthWithoutPadding(size_t decodedLength)
	{
		return (decodedLength * 8 + 5) / 6;
	}

	static constexpr size_t GetEncodedLengthWithPadding(size_t decodedLength)
	{
		return ((GetEncodedLengthWithoutPadding(decodedLength) + 3) / 4) * 4;
	}

	static size_t CountPadding(const std::string &encoded)
	{
		size_t padding = 0;
		size_t len = encoded.length();
		if (len == 0)
			return 0;

		if (encoded[len - 1] == '=')
			++padding;

		if (len == 1)
			return padding;

		if (encoded[len - 2] == '=')
			++padding;

		return padding;
	}

	static size_t GetDecodedLength(const std::string &encoded)
	{
		size_t padding = CountPadding(encoded);
		return (encoded.length() - padding) * 3 / 4;
	}

	static std::string encode(size_t(*func)(const unsigned char *in, size_t inlen, unsigned char *out), const std::string &bytes)
	{
		size_t encLen = GetEncodedLengthWithPadding(bytes.length());
		std::string encoded(encLen, '=');
		func((const unsigned char*)&bytes[0], bytes.length(), (unsigned char*)&encoded[0]);
		return encoded;
	}

	static std::string decode(size_t(*func)(const unsigned char *in, size_t inlen, unsigned char *out), const std::string &encoded)
	{
		size_t decLen = GetDecodedLength(encoded);
		std::string decoded(decLen, '\0');
		func((const unsigned char*)&encoded[0], encoded.length(), (unsigned char*)&decoded[0]);
		return decoded;
	}
};

struct Turbob64
{
	static size_t encode_into(const std::string &bytes, char *output)
	{
		return tb64v256enc((const unsigned char*)bytes.data(), bytes.size(),
			(unsigned char*)output);
	}

	static size_t decode_into(const std::string &base64, unsigned char *output)
	{
		return tb64v256dec((const unsigned char*)base64.data(),
			base64.size(), output);
	}

	std::string encode(const std::string &bytes)
	{
		return B64_Adapt::encode(tb64v256enc, bytes);
	}

	std::string decode(const std::string &base64)
	{
		return B64_Adapt::decode(tb64v256dec, base64);
	}
};

BASE64_REGISTER_ENCODER(Turbob64);
BASE64_REGISTER_RAW_ENCODER(Turbob64);
BASE64_REGISTER_DECODER(Turbob64);
BASE64_REGISTER_RAW_DECODER(Turbob64);
