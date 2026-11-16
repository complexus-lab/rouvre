#include <signing/signing.h>

static std::string from_hex (std::string const& hex)
{
	std::string res;
	for(size_t i = 0; i + 1 < hex.size(); i += 2)
		res += (char)strtol(hex.substr(i, 2).c_str(), nullptr, 16);
	return res;
}

static std::string from_base64 (std::string const& src)
{
	static std::string const kAlphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	std::string res;
	uint32_t bits = 0;
	int count = 0;
	for(char ch : src)
	{
		size_t value = kAlphabet.find(ch);
		if(value == std::string::npos)
			continue;
		bits = (bits << 6) | value;
		if((count += 6) >= 8)
			res += (char)((bits >> (count -= 8)) & 0xFF);
	}
	return res;
}

// RFC 8032, section 7.1, tests 1 to 3.

void test_rfc8032_vectors ()
{
	struct { char const* key; char const* message; char const* signature; } const vectors[] = {
		{ "d75a980182b10ab7d54bfed3c964073a0ee172f3daa62325af021a68f707511a", "",     "e5564300c360ac729086e2cc806e828a84877f1eb8e5d974d873e065224901555fb8821590a33bacc61e39701cf9b46bd25bf5f0595bbe24655141438e7a100b" },
		{ "3d4017c3e843895a92b70aa74d1b7ebc9c982ccf2ec4968cc0cd55f12af4660c", "72",   "92a009a9f0d4cab8720e820b5f642540a2b27b5416503f8fb3762223ebdb69da085ac1e43e15996e458f3613d0f11d8c387b2eaeb4302aeeb00d291612bb0c00" },
		{ "fc51cd8e6218a1a38da47ed00230f0580816ed13ba3303ac5deb911548908025", "af82", "6291d657deec24024827e69c3abe01a30ce548a284743a445e3680d7db5ac3ac18ff9b538d16f290ae67f760984dc6594a7c15e9716ed28dc027beceea1ec40a" },
	};

	for(auto const& vector : vectors)
	{
		std::string const key       = from_hex(vector.key);
		std::string const message   = from_hex(vector.message);
		std::string const signature = from_hex(vector.signature);

		OAK_ASSERT(signing::verify(message, signature, (uint8_t const*)key.data()));

		std::string tampered = signature;
		tampered[10] ^= 1;
		OAK_ASSERT(!signing::verify(message, tampered, (uint8_t const*)key.data()));

		OAK_ASSERT(!signing::verify(message + "x", signature, (uint8_t const*)key.data()));
		OAK_ASSERT(!signing::verify(message, signature.substr(0, 63), (uint8_t const*)key.data()));
	}
}

void test_release_key_matches_repository ()
{
	// Tests run from the root of the source tree.
	FILE* fp = fopen("keys/release-signing.pub.pem", "r");
	OAK_ASSERT(fp != nullptr);

	std::string base64;
	char line[256];
	while(fgets(line, sizeof(line), fp))
	{
		if(strncmp(line, "-----", 5) != 0)
			base64 += line;
	}
	fclose(fp);

	std::string const der = from_base64(base64);
	OAK_ASSERT_EQ(der.size(), 44);
	OAK_ASSERT(der.substr(0, 12) == from_hex("302a300506032b6570032100")); // Ed25519 SubjectPublicKeyInfo
	OAK_ASSERT(der.substr(12) == std::string((char const*)signing::kReleasePublicKey, 32));
}

void test_supersedes ()
{
	std::string const older = "2026-11-09T04:17:02Z", newer = "2026-11-16T12:53:38Z";

	OAK_ASSERT(signing::supersedes(newer, older));
	OAK_ASSERT(signing::supersedes(newer, newer));
	OAK_ASSERT(!signing::supersedes(older, newer));

	// Nothing to protect yet: first download, or a file from before the time was recorded.
	OAK_ASSERT(signing::supersedes(newer, ""));
	OAK_ASSERT(signing::supersedes("", ""));
	OAK_ASSERT(signing::supersedes(newer, "not a time"));

	// A file without a usable time never replaces one that has it.
	OAK_ASSERT(!signing::supersedes("", older));
	OAK_ASSERT(!signing::supersedes("2026-11-16 12:53:38", older));
	OAK_ASSERT(!signing::supersedes("9999", older));
}

void test_sha256 ()
{
	OAK_ASSERT_EQ(signing::sha256("", 0),    "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
	OAK_ASSERT_EQ(signing::sha256("abc", 3), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");

	signing::sha256_t digest;
	digest.update("a", 1);
	digest.update("bc", 2);
	OAK_ASSERT_EQ(digest.hex_digest(), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}
