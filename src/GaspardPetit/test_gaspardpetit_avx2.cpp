#include "../Base64SurveyRegistry.hpp"
#include "../../libs/gaspardpetit/base64/base64.h"
#include "../../libs/gaspardpetit/base64/base64_avx2.h"

#include <algorithm>
#include <string>

struct GaspardPetitAVX2
{
    static std::string encode(const std::string& bytes)
    {
        std::string result(base64_encoded_size(bytes.size()), '\0');
        base64_avx2_encode(
            reinterpret_cast<const unsigned char*>(bytes.data()),
            bytes.size(), result.data());
        return result;
    }

    static std::string decode(const std::string& encoded)
    {
        std::string result(base64_decoded_max_size(encoded.size()), '\0');
        const size_t size = base64_avx2_decode(
            reinterpret_cast<const unsigned char*>(encoded.data()),
            encoded.size(),
            reinterpret_cast<unsigned char*>(result.data()));
        if (size == BASE64_ERROR)
            return {};
        result.resize(size);
        return result;
    }
};

TEST(GaspardPetitAVX2, binary_round_trip)
{
    for (size_t length = 0; length <= 1024; ++length)
    {
        std::string input(length, '\0');
        unsigned value = 1;
        for (char& byte : input)
        {
            value = value * 1103515245U + 12345U;
            byte = static_cast<char>(value >> 24);
        }

        const std::string encoded = GaspardPetitAVX2::encode(input);
        const std::string decoded = GaspardPetitAVX2::decode(encoded);
        if (input != decoded)
        {
            const size_t mismatch = std::mismatch(
                input.begin(), input.end(), decoded.begin(), decoded.end()).first
                - input.begin();
            FAIL() << "input length " << length << ", mismatch " << mismatch
                   << ", encoded quartet "
                   << encoded.substr((mismatch / 3) * 4, 4)
                   << ", expected "
                   << static_cast<unsigned>(static_cast<unsigned char>(input[mismatch]))
                   << ", got "
                   << static_cast<unsigned>(static_cast<unsigned char>(decoded[mismatch]));
        }
    }
}

TEST(GaspardPetitAVX2, decode_simd_block)
{
    std::string encoded(104, 'A');
    encoded.replace(8, 4, "ynow");
    std::string decoded(base64_decoded_max_size(encoded.size()), '\0');
    const size_t size = base64_avx2_decode(
        reinterpret_cast<const unsigned char*>(encoded.data()), encoded.size(),
        reinterpret_cast<unsigned char*>(decoded.data()));
    decoded.resize(size);
    EXPECT_EQ(0xca, static_cast<unsigned char>(decoded[6]));
    EXPECT_EQ(0x7a, static_cast<unsigned char>(decoded[7]));
    EXPECT_EQ(0x30, static_cast<unsigned char>(decoded[8]));
}

TEST(GaspardPetitAVX2, validates_every_simd_lane)
{
    std::string encoded(104, 'A');
    std::string decoded(base64_decoded_max_size(encoded.size()), '\0');
    for (size_t position = 0; position < 96; ++position)
    {
        for (unsigned byte = 0; byte < 256; ++byte)
        {
            const bool valid = (byte >= 'A' && byte <= 'Z') ||
                               (byte >= 'a' && byte <= 'z') ||
                               (byte >= '0' && byte <= '9') ||
                               byte == '+' || byte == '/' ||
                               byte == '-' || byte == '_';
            if (valid)
                continue;
            encoded[position] = static_cast<char>(byte);
            EXPECT_EQ(BASE64_ERROR, base64_avx2_decode(
                reinterpret_cast<const unsigned char*>(encoded.data()),
                encoded.size(),
                reinterpret_cast<unsigned char*>(decoded.data())))
                << "position " << position << ", byte " << byte;
        }
        encoded[position] = 'A';
    }
}

TEST(GaspardPetitAVX2, standard_decoder_rejects_url_safe_alphabet)
{
    std::string encoded(104, 'A');
    std::string decoded(base64_decoded_max_size(encoded.size()), '\0');
    for (size_t position = 0; position < 96; ++position)
    {
        for (unsigned byte = 0; byte < 256; ++byte)
        {
            const bool valid = (byte >= 'A' && byte <= 'Z') ||
                               (byte >= 'a' && byte <= 'z') ||
                               (byte >= '0' && byte <= '9') ||
                               byte == '+' || byte == '/';
            if (valid)
                continue;
            encoded[position] = static_cast<char>(byte);
            EXPECT_EQ(BASE64_ERROR, base64_avx2_decode_standard(
                reinterpret_cast<const unsigned char*>(encoded.data()),
                encoded.size(),
                reinterpret_cast<unsigned char*>(decoded.data())))
                << "position " << position << ", byte " << byte;
        }
        encoded[position] = 'A';
    }
}
