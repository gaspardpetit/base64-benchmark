#include "../Base64SurveyRegistry.hpp"

#include <etl/base64_decoder.h>
#include <etl/base64_encoder.h>

#include <stdexcept>
#include <string>

struct EtlCpp
{
    static constexpr size_t EncodeBufferSize = 4096U;
    static constexpr size_t DecodeBufferSize = 4095U;

    static std::string encode(const std::string& bytes)
    {
        std::string result;
        result.reserve(((bytes.size() + 2U) / 3U) * 4U);

        auto append = [&result](const etl::span<const char>& chunk)
        {
            result.append(chunk.begin(), chunk.end());
        };

        etl::base64_rfc4648_padding_encoder<EncodeBufferSize>::callback_type callback = append;
        etl::base64_rfc4648_padding_encoder<EncodeBufferSize> encoder(callback);
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

        etl::base64_rfc4648_padding_decoder<DecodeBufferSize>::callback_type callback = append;
        etl::base64_rfc4648_padding_decoder<DecodeBufferSize> decoder(callback);
        if (!decoder.decode_final(base64.begin(), base64.end()))
        {
            throw std::runtime_error("ETL base64 decoding failed");
        }

        return result;
    }
};

BASE64_REGISTER_ENCODER_NAMED(EtlCpp, "ETL-20.49.0");
BASE64_REGISTER_DECODER_NAMED(EtlCpp, "ETL-20.49.0");
