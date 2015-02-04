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
#include "Crypto.hpp"

using namespace LIBAWS;


std::string Crypto::base64Encode(const std::string &msg){
	std::string encoded;

	StringSource(reinterpret_cast<const byte*>(msg.data()), msg.size(), true,
	new Base64Encoder(
		new StringSink(encoded)
		) 
	);		
	return encoded;
}

std::string Crypto::base64Decode(const std::string &msg){
	std::string encoded;

	StringSource(reinterpret_cast<const byte*>(msg.data()), msg.size(), true,
	new Base64Decoder(
		new StringSink(encoded)
		) 
	);		
	return encoded;
}

std::string Crypto::hexDump(std::array<byte, 32> &mac) {
	std::string encoded;

	StringSource(mac.data(), 32, true,
	new HexEncoder(
		new StringSink(encoded)
		) 
	);		
	return encoded;
}

std::array<byte, 32> Crypto::sign(std::array<byte, 32> &key, const std::string &msg, size_t size) {
	std::array<byte, 32> mac2;

	std::string mac;
	HMAC< SHA256 > hmac(key.data(), size);		

	StringSource(msg, true, 
		new HashFilter(hmac,
			new StringSink(mac)
		)      
	);
	
	std::copy(mac.begin(), mac.end(), mac2.data());
return std::move(mac2);

}

std::array<byte, 32> Crypto::sign(const std::string &key, const std::string &msg, size_t size) {
	std::array<byte, 32> mac2;
	std::string mac;
	HMAC< SHA256 > hmac(reinterpret_cast<byte*>(const_cast<char *>(key.c_str())), key.length());		

	StringSource(msg, true, 
		new HashFilter(hmac,
			new StringSink(mac)
		)      
	);
 	std::copy(mac.begin(), mac.end(), mac2.data()); //but the raw data into the array
	
	return std::move(mac2);
}

std::string Crypto::shaDigest(const std::string &key) {	
	std::array<byte, 64> out;

	CryptoPP::SHA256().CalculateDigest(out.data(), reinterpret_cast<const byte*>(key.c_str()), key.size());
	
	std::stringstream ss;

	for (auto byte : out) {
		ss <<  std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(byte);
	}
	return ss.str().substr(0, 64);
}


std::array<byte, 32> Crypto::getSignatureKey(const std::string &key, const std::string &dateStamp, const std::string &regionName, const std::string &serviceName) {
    std::array<byte, 32> kDate = sign("AWS4" + key, dateStamp, key.size() + 4);	   
    std::array<byte, 32> kRegion = sign(kDate, regionName);  
    std::array<byte, 32> kService = sign(kRegion, serviceName);
    std::array<byte, 32> kSigning = sign(kService, "aws4_request");
    return std::move(kSigning);
}

