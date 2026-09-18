#include "../Base64SurveyRegistry.hpp"
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>



using namespace boost::archive::iterators;

struct Boost
{
    std::string encode(const std::string &bytes)
    {
        using namespace boost::archive::iterators;
        using It = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
        auto base64 = std::string(It(bytes.begin()), It(bytes.end()));
        // Add padding.
        return base64.append((3 - bytes.size() % 3) % 3, '=');
    }

    std::string decode(const std::string &base64)
	{
        using namespace boost::archive::iterators;
        using It = transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
        size_t padding = 0;
        if (!base64.empty() && base64.back() == '=') {
            padding = 1;
            if (base64.size() > 1 && base64[base64.size() - 2] == '=')
                padding = 2;
        }
        return std::string(
            It(base64.begin()), It(base64.end() - padding));
    }
};

BASE64_REGISTER_ENCODER_NAMED(Boost, "Boost-1.92.0");
BASE64_REGISTER_DECODER_NAMED(Boost, "Boost-1.92.0");
