#include "signing.h"
#include <Monocypher/monocypher-ed25519.h>

namespace signing
{
	// Raw form of keys/release-signing.pub.pem (the last 32 bytes of its DER
	// encoding). The tests check that both stay the same key.
	uint8_t const kReleasePublicKey[32] = {
		0x9c, 0x67, 0x42, 0xe3, 0xee, 0xa0, 0x84, 0x1e,
		0xc5, 0x6e, 0x14, 0x08, 0x8d, 0xea, 0x55, 0xc4,
		0xae, 0x15, 0x6f, 0x3a, 0x5d, 0xaa, 0xe8, 0x8b,
		0x27, 0x98, 0x30, 0x26, 0xe7, 0x9e, 0x65, 0xba
	};

	bool verify (void const* data, size_t length, void const* signature, size_t signatureLength, uint8_t const* publicKey)
	{
		if((!data && length) || !signature || signatureLength != 64 || !publicKey)
			return false;
		return crypto_ed25519_check((uint8_t const*)signature, publicKey, (uint8_t const*)data, length) == 0;
	}

	bool verify (std::string const& data, std::string const& signature, uint8_t const* publicKey)
	{
		return verify(data.data(), data.size(), signature.data(), signature.size(), publicKey);
	}

	// ===========
	// = SHA-256 =
	// ===========

	sha256_t::sha256_t ()
	{
		CC_SHA256_Init(&_context);
	}

	void sha256_t::update (void const* data, size_t length)
	{
		CC_SHA256_Update(&_context, data, (CC_LONG)length);
	}

	std::string sha256_t::hex_digest ()
	{
		uint8_t digest[CC_SHA256_DIGEST_LENGTH];
		CC_SHA256_Final(digest, &_context);

		static char const kHexDigits[] = "0123456789abcdef";
		std::string res;
		for(uint8_t byte : digest)
		{
			res += kHexDigits[byte >> 4];
			res += kHexDigits[byte & 0xF];
		}
		return res;
	}

	std::string sha256 (void const* data, size_t length)
	{
		sha256_t digest;
		digest.update(data, length);
		return digest.hex_digest();
	}

} /* signing */
