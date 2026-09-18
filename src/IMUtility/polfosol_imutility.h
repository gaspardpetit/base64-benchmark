#include <stdbool.h>
#include <cstdint>
#include <cstring>
#include <string>
namespace polfosol_imutility {

#include "IMUtility/Inc/base64.h"
#include "IMUtility/Src/base64.c"

std::string Base64_encode(unsigned char *bytes, size_t len)
{
	size_t d = len % 3;
	std::string str64(4 * (int(d > 0) + len / 3), '=');

	Base64_encode((char*)bytes, len, (char*)&str64[0], str64.size());

	return str64;
}

std::string Base64_decode(unsigned char *p, size_t len)
{
	int pad = len > 0 && (len % 4 || p[len - 1] == '=');
	const size_t L = ((len + 3) / 4 - pad) * 4;
	if ((len > (L + 2U)) && (p[L + 2U] != '='))
		++pad;
	std::string str(L / 4 * 3 + pad, '\0');

	Base64_decode((char*)p, len, (unsigned char*)&str[0], str.size());

	return str;
}


} // namespace polfosol
