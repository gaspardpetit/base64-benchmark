#include "../Base64SurveyRegistry.hpp"

#ifndef GASPARDPETIT_COMPILED
#define BASE64_HEADER_ONLY
#endif
#include "../../libs/gaspardpetit/base64/base64.h"

#include <string>

struct GaspardPetit
{
    static std::string encode(const std::string& bytes)
    {
        std::string result(((bytes.size() + 2U) / 3U) * 4U, '\0');
        base64_encode(
            reinterpret_cast<const unsigned char*>(bytes.data()),
            bytes.size(), result.data());
        return result;
    }

    static std::string decode(const std::string& encoded)
    {
        std::string result(base64_decoded_max_size(encoded.size()), '\0');
        const size_t size = base64_decode(
            reinterpret_cast<const unsigned char*>(encoded.data()),
            encoded.size(),
            reinterpret_cast<unsigned char*>(result.data()));
        if (size == BASE64_ERROR)
            return {};
        result.resize(size);
        return result;
    }
};

BASE64_REGISTER_ENCODER(GaspardPetit);
BASE64_REGISTER_DECODER(GaspardPetit);
