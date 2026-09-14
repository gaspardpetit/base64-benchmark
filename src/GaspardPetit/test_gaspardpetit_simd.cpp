/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Gaspard Petit
 */

#include "../Base64SurveyRegistry.hpp"

#define base64_encode gaspardpetit_simd_encode
#define base64_decode gaspardpetit_simd_decode
#define base64_decode_unchecked gaspardpetit_simd_decode_unchecked
#include "../../libs/gaspardpetit/base64/base64.h"

#include <algorithm>
#include <string>

namespace {

struct GaspardPetit_SIMD
{
    static std::string encode(const std::string& bytes)
    {
        std::string result(base64_encoded_size(bytes.size()), '\0');
        base64_encode(reinterpret_cast<const unsigned char*>(bytes.data()),
                      bytes.size(), result.data());
        return result;
    }

    static std::string decode(const std::string& encoded)
    {
        std::string result(base64_decoded_max_size(encoded.size()), '\0');
        const size_t size = base64_decode(
            reinterpret_cast<const unsigned char*>(encoded.data()),
            encoded.size(), reinterpret_cast<unsigned char*>(result.data()),
            0);
        if (size == BASE64_ERROR)
            return {};
        result.resize(size);
        return result;
    }
};

struct GaspardPetit_SIMD_Unchecked
{
    static std::string decode(const std::string& encoded)
    {
        std::string result(base64_decoded_max_size(encoded.size()), '\0');
        const size_t size = base64_decode_unchecked(
            reinterpret_cast<const unsigned char*>(encoded.data()),
            encoded.size(), reinterpret_cast<unsigned char*>(result.data()),
            0);
        if (size == BASE64_ERROR)
            return {};
        result.resize(size);
        return result;
    }
};

BASE64_REGISTER_ENCODER(GaspardPetit_SIMD);
BASE64_REGISTER_DECODER(GaspardPetit_SIMD);
BASE64_REGISTER_DECODER(GaspardPetit_SIMD_Unchecked);

TEST(GaspardPetit_SIMD, binary_round_trip)
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

        const std::string encoded = GaspardPetit_SIMD::encode(input);
        const std::string decoded = GaspardPetit_SIMD::decode(encoded);
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

TEST(GaspardPetit_SIMD, decode_simd_block)
{
    std::string encoded(104, 'A');
    encoded.replace(8, 4, "ynow");
    const std::string decoded = GaspardPetit_SIMD::decode(encoded);
    EXPECT_EQ(0xca, static_cast<unsigned char>(decoded[6]));
    EXPECT_EQ(0x7a, static_cast<unsigned char>(decoded[7]));
    EXPECT_EQ(0x30, static_cast<unsigned char>(decoded[8]));
}

TEST(GaspardPetit_SIMD, validates_every_simd_lane)
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
            EXPECT_EQ(BASE64_ERROR, base64_decode(
                reinterpret_cast<const unsigned char*>(encoded.data()),
                encoded.size(),
                reinterpret_cast<unsigned char*>(decoded.data()), 1))
                << "position " << position << ", byte " << byte;
        }
        encoded[position] = 'A';
    }
}

TEST(GaspardPetit_SIMD, standard_decoder_rejects_url_safe_alphabet)
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
            EXPECT_EQ(BASE64_ERROR, base64_decode(
                reinterpret_cast<const unsigned char*>(encoded.data()),
                encoded.size(),
                reinterpret_cast<unsigned char*>(decoded.data()), 0))
                << "position " << position << ", byte " << byte;
        }
        encoded[position] = 'A';
    }
}

} // namespace
