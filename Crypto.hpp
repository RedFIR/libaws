#ifndef __LIB_AWS_CRYPTO__
#define __LIB_AWS_CRYPTO__

#include "Utils.hpp"
#include <iomanip>      // std::setw


#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include "hmac.h"
#include <openssl/hmac.h>
#include <secblock.h>
#include "filters.h"
#include "hex.h"
#include "base64.h"
#include "hex.h"


namespace LIBAWS {
	namespace Crypto {
		using CryptoPP::Base64Encoder;
		using CryptoPP::HexEncoder;
		using CryptoPP::HMAC;
		using CryptoPP::SHA256;
		using CryptoPP::StringSink;
		using CryptoPP::StringSource;
		using CryptoPP::HashFilter;
		using CryptoPP::HashVerificationFilter;
		using CryptoPP::HexEncoder;
		using CryptoPP::HexDecoder;

	std::string hexDump(std::array<byte, 32> &mac);

	std::array<byte, 32> sign(std::array<byte, 32> &key, const std::string &msg, size_t size = 32);

	std::array<byte, 32> sign(const std::string &key, const std::string &msg, size_t size = 32);

	std::string shaDigest(const std::string &key = "");

	std::array<byte, 32> getSignatureKey(const std::string &key, const std::string &dateStamp, const std::string &regionName, const std::string &serviceName);



	};

};


#endif