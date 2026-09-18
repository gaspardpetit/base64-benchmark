#include "../Base64SurveyRegistry.hpp"
#include <gtest/gtest.h>
#include "singleheader-9.2.0/simdutf.h"
#include "singleheader-9.2.0/simdutf.cpp"


struct simdutfb64
{
  static size_t encode_into(const std::string& bytes, char* output)
  {
    return simdutf::binary_to_base64(bytes.data(), bytes.size(), output);
  }

  static size_t decode_into(const std::string& base64, unsigned char* output)
  {
    const simdutf::result r = simdutf::base64_to_binary(
      base64.data(), base64.size(), reinterpret_cast<char*>(output));
    return r.error ? 0 : r.count;
  }

  std::string encode(const std::string& bytes)
  {
    // There is no string constructor with uninitialised values and resize uses value-initialized characters
    std::string buffer(simdutf::base64_length_from_binary(bytes.size()),char(0));
    simdutf::binary_to_base64(bytes.data(), bytes.size(), buffer.data());
    return buffer;
  }
  
  std::string decode(const std::string& base64)
  {
    // There is no string constructor with uninitialised values and resize uses value-initialized characters
    std::string buffer(simdutf::maximal_binary_length_from_base64(base64.data(), base64.size()),char(0));
    simdutf::result r = simdutf::base64_to_binary(base64.data(), base64.size(), buffer.data());
    if(r.error) {
      // We have some error, r.count tells you where the error was encountered in the input if
      // the error is INVALID_BASE64_CHARACTER. If the error is BASE64_INPUT_REMAINDER, then
      // a single valid base64 character remained, and r.count contains the number of bytes decoded.
      buffer.resize(0);
    } else {
      buffer.resize(r.count); // resize the buffer according to actual number of bytes
    }
    return buffer;
  }
};

struct simdutf_scalar
{
  static const simdutf::implementation& implementation()
  {
    static const simdutf::implementation* fallback =
      simdutf::get_available_implementations()["fallback"];
    return *fallback;
  }

  static size_t encode_into(const std::string& bytes, char* output)
  {
    return implementation().binary_to_base64(
      bytes.data(), bytes.size(), output, simdutf::base64_default);
  }

  static size_t decode_into(const std::string& base64, unsigned char* output)
  {
    const simdutf::result r = implementation().base64_to_binary(
      base64.data(), base64.size(), reinterpret_cast<char*>(output),
      simdutf::base64_default);
    return r.error ? 0 : r.count;
  }

  std::string encode(const std::string& bytes)
  {
    std::string buffer(
      simdutf::base64_length_from_binary(bytes.size()), char(0));
    buffer.resize(encode_into(bytes, buffer.data()));
    return buffer;
  }

  std::string decode(const std::string& base64)
  {
    std::string buffer(
      simdutf::maximal_binary_length_from_base64(
        base64.data(), base64.size()),
      char(0));
    const simdutf::result r = implementation().base64_to_binary(
      base64.data(), base64.size(), buffer.data(), simdutf::base64_default);
    if (r.error)
      return {};
    buffer.resize(r.count);
    return buffer;
  }
};

BASE64_REGISTER_ENCODER_NAMED(simdutfb64, "simdutf-9.2.0");
BASE64_REGISTER_DECODER_NAMED(simdutfb64, "simdutf-9.2.0");
BASE64_REGISTER_RAW_ENCODER_NAMED(simdutfb64, "simdutf-9.2.0");
BASE64_REGISTER_RAW_DECODER_NAMED(simdutfb64, "simdutf-9.2.0");
BASE64_REGISTER_ENCODER_NAMED(simdutf_scalar, "simdutf-scalar-9.2.0");
BASE64_REGISTER_DECODER_NAMED(simdutf_scalar, "simdutf-scalar-9.2.0");
BASE64_REGISTER_RAW_ENCODER_NAMED(simdutf_scalar, "simdutf-scalar-9.2.0");
BASE64_REGISTER_RAW_DECODER_NAMED(simdutf_scalar, "simdutf-scalar-9.2.0");
