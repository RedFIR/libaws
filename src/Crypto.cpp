#include "Crypto.hpp"

using namespace LIBAWS;

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

