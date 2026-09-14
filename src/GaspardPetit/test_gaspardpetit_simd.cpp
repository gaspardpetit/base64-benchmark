/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Gaspard Petit
 */

#include "../Base64SurveyRegistry.hpp"

#if defined(GASPARDPETIT_SIMD)
#define base64_encode gaspardpetit_encode
#define base64_decode gaspardpetit_decode
#define base64_decode_unchecked gaspardpetit_decode_unchecked
#endif
#include "../../libs/gaspardpetit/base64/base64.h"

#include <string>

#if defined(GASPARDPETIT_SIMD)
namespace {

struct GaspardPetit_SIMD {
    static std::string encode(const std::string& bytes) {
        std::string result(base64_encoded_size(bytes.size()), '\0');
        base64_encode(reinterpret_cast<const unsigned char*>(bytes.data()),
                      bytes.size(), result.data());
        return result;
    }

    static std::string decode(const std::string& encoded) {
        std::string result(base64_decoded_max_size(encoded.size()), '\0');
        const size_t size = base64_decode(
            reinterpret_cast<const unsigned char*>(encoded.data()),
            encoded.size(), reinterpret_cast<unsigned char*>(result.data()), false);
        if (size == BASE64_ERROR)
            return {};
        result.resize(size);
        return result;
    }
};

struct GaspardPetit_SIMD_Unchecked {
    static std::string decode(const std::string& encoded) {
        std::string result(base64_decoded_max_size(encoded.size()), '\0');
        const size_t size = base64_decode_unchecked(
            reinterpret_cast<const unsigned char*>(encoded.data()),
            encoded.size(), reinterpret_cast<unsigned char*>(result.data()), false);
        if (size == BASE64_ERROR)
            return {};
        result.resize(size);
        return result;
    }
};

BASE64_REGISTER_ENCODER(GaspardPetit_SIMD);
BASE64_REGISTER_DECODER(GaspardPetit_SIMD);
BASE64_REGISTER_DECODER(GaspardPetit_SIMD_Unchecked);

} // namespace
#endif
