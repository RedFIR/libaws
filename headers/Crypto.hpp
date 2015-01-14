/*
 * Copyright (c) 2014 Smyle SAS
 * http: *www.deepomatic.com/
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
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

	// http://docs.aws.amazon.com/general/latest/gr/signature-v4-examples.html#signature-v4-examples-python
	std::string hexDump(std::array<byte, 32> &mac);

	std::array<byte, 32> sign(std::array<byte, 32> &key, const std::string &msg, size_t size = 32);

	std::array<byte, 32> sign(const std::string &key, const std::string &msg, size_t size = 32);

	std::array<byte, 32> getSignatureKey(const std::string &key, const std::string &dateStamp, const std::string &regionName, const std::string &serviceName);

	// Create payload hash (check aws api documentation)
	std::string shaDigest(const std::string &key = "");


	};

};


#endif