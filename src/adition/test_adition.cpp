#include "adition.hpp"
#include <gtest/gtest.h>
#include "../Base64SurveyRegistry.hpp"


struct Adition
{
	static size_t decode_into(const std::string &base64, unsigned char *output)
	{
		CBase64Parser parser;
		return parser.DecodeChunk(base64.c_str(), base64.size(), output);
	}

	CBase64Parser p;
	std::string encode(const std::string &bytes)
	{
		std::string out;
        uint32_t eLen=p.GetEncodeLen(bytes.length());
        out.resize(eLen);

        p.EncodeChunk((unsigned char*)bytes.data(), bytes.length(),&out[0]);
        return out;
	}

	std::string decode(const std::string &base64)
	{
        std::string out;
		uint32_t eLen=p.GetDecodeExpectedLen(base64.length());
        out.resize(eLen);
		eLen=p.DecodeChunk(base64.c_str(), base64.length(),reinterpret_cast<uint8_t*>(&out[0]));
		out.resize(eLen);
        return out;
	}
};

BASE64_REGISTER_ENCODER(Adition);
BASE64_REGISTER_DECODER(Adition);
BASE64_REGISTER_RAW_DECODER(Adition);
