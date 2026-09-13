#include "../Base64SurveyRegistry.hpp"

#include <etl/base64_decoder.h>
#include <etl/base64_encoder.h>

#include <stdexcept>
#include <string>

struct EtlCpp
{
    static std::string encode(const std::string& bytes)
    {
        std::string result;
        result.reserve(((bytes.size() + 2U) / 3U) * 4U);

        auto append = [&result](const etl::span<const char>& chunk)
        {
            result.append(chunk.begin(), chunk.end());
        };

        etl::base64_rfc4648_padding_encoder<>::callback_type callback = append;
        etl::base64_rfc4648_padding_encoder<> encoder(callback);
        if (!encoder.encode_final(bytes.begin(), bytes.end()))
        {
            throw std::runtime_error("ETL base64 encoding failed");
        }

        return result;
    }

    static std::string decode(const std::string& base64)
    {
        std::string result;
        result.reserve((base64.size() / 4U) * 3U);

        auto append = [&result](const etl::span<const unsigned char>& chunk)
        {
            result.append(reinterpret_cast<const char*>(chunk.data()), chunk.size());
        };

        etl::base64_rfc4648_padding_decoder<>::callback_type callback = append;
        etl::base64_rfc4648_padding_decoder<> decoder(callback);
        if (!decoder.decode_final(base64.begin(), base64.end()))
        {
            throw std::runtime_error("ETL base64 decoding failed");
        }

        return result;
    }
};

BASE64_REGISTER_ENCODER(EtlCpp);
BASE64_REGISTER_DECODER(EtlCpp);
