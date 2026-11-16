#ifndef SIGNING_H_4QJ7XW2B
#define SIGNING_H_4QJ7XW2B

#include <CommonCrypto/CommonDigest.h>
#include <cstdint>
#include <string>

namespace signing
{
	// Ed25519 public key that signs Rouvre releases and the bundle index.
	extern uint8_t const kReleasePublicKey[32];

	// True when signature is a valid Ed25519 signature of data by publicKey.
	bool verify (void const* data, size_t length, void const* signature, size_t signatureLength, uint8_t const* publicKey = kReleasePublicKey);
	bool verify (std::string const& data, std::string const& signature, uint8_t const* publicKey = kReleasePublicKey);

	// A signature stays valid for ever, so a server can answer with an older
	// signed file. Signed files carry the UTC time of their creation as
	// YYYY-MM-DDTHH:MM:SSZ (mirror.created in the bundle index). This is true
	// when candidate may replace current: current carries no such time, or
	// candidate carries one that is not earlier.
	bool supersedes (std::string const& candidate, std::string const& current);

	// SHA-256 digests as lowercase hexadecimal, the form used in the bundle index.
	struct sha256_t
	{
		sha256_t ();
		void update (void const* data, size_t length);
		std::string hex_digest ();

	private:
		CC_SHA256_CTX _context;
	};

	std::string sha256 (void const* data, size_t length);

} /* signing */

#endif /* end of include guard: SIGNING_H_4QJ7XW2B */
