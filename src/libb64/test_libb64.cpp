#define BUFFERSIZE 65536
#include "libb64/include/b64/encode.h"
#include "libb64/include/b64/decode.h"
#include <gtest/gtest.h>
#include "../Base64SurveyRegistry.hpp"


struct Libb64
{
	static base64::encoder e;
	static base64::decoder d;

	std::string encode(const std::string &bytes)
	{
		std::istringstream istrm(bytes);
		std::ostringstream ostrm;
		e.encode(istrm, ostrm);
		return std::move(ostrm.str());
	}

	std::string decode(const std::string &base64)
	{
		std::istringstream istrm(base64);
		std::ostringstream ostrm;
		d.decode(istrm, ostrm);
		return std::move(ostrm.str());
	}
};

base64::encoder Libb64::e;
base64::decoder Libb64::d;

BASE64_REGISTER_ENCODER_NAMED(Libb64, "libb64-2.0.0.1-9-g64ab5ed");
BASE64_REGISTER_DECODER_NAMED(Libb64, "libb64-2.0.0.1-9-g64ab5ed");
