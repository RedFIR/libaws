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
#include "SQS.hpp"
#include "Utils.hpp"
#include "Crypto.hpp"

using namespace LIBAWS;

SQS::SQS(const std::string & awsKeyID, const std::string& awsSecretKey, const std::string &region) : _awsKeyID(awsKeyID),
_awsSecretKey(awsSecretKey), _method("GET"), _service("sqs"), _region(region){
	this->_host = "sqs." + region + ".amazonaws.com";
	this->_endpoint = "https://" + this->_host; 
}

const std::string &SQS::getQueue(const std::string &queueName, bool create) const {
	auto it = this->_queueMap.find(queueName);
	if (it != this->_queueMap.end()) {
		return it->second;
	}

	if (create == false) {
		return std::move(std::string(""));
	}


	std::time_t t = std::time(nullptr);
	std::string amz_date =  Utils::getAmzDate(t); 
	std::string datestamp = Utils::getDatestamp(t);

	std::string canonical_uri("/");
	std::string canonical_headers = "host:" + this->_host + "\n";
	std::string signed_headers = "host";

	std::string algorithm = "AWS4-HMAC-SHA256";
	std::string credential_scope = datestamp + "/" + this->_region + "/" + this->_service + "/" + "aws4_request";

	std::string canonical_querystring = "Action=CreateQueue&QueueName=" + queueName; // TODO: verif queueName
	canonical_querystring += "&X-Amz-Algorithm=AWS4-HMAC-SHA256";
	std::string tmp = this->_awsKeyID + "/" + credential_scope;
	canonical_querystring += "&X-Amz-Credential=" + Utils::replace(tmp.begin(), tmp.end(), '/', "%2F"); //urllib.quote_plus
	canonical_querystring += "&X-Amz-Date=" + amz_date;
	canonical_querystring += "&X-Amz-Expires=30";
	canonical_querystring += "&X-Amz-SignedHeaders=" + signed_headers;

	std::string payload_hash = Crypto::shaDigest();


	std::string canonical_request = this->_method + '\n' + canonical_uri + '\n' + canonical_querystring + '\n' + canonical_headers + '\n' + signed_headers + '\n' + payload_hash;

	std::string string_to_sign = algorithm + '\n' +  amz_date + '\n' +  credential_scope + '\n' +  Crypto::shaDigest(canonical_request);

	std::array<byte, 32> signing_key = Crypto::getSignatureKey(this->_awsSecretKey, datestamp, this->_region, this->_service);

	auto _sign = Crypto::sign(signing_key, string_to_sign);
	std::string signature = Crypto::hexDump(_sign);
	std::transform(signature.begin(), signature.end(), signature.begin(), ::tolower);


	canonical_querystring += "&X-Amz-Signature=" + signature;
	std::string request_url = this->_endpoint + "?" + canonical_querystring;

	std::stringstream ss;
	Utils::executeRequest(request_url, ss);
	std::string queueUrl = Utils::getQueueUrl(ss);
	std::string canonicalUri = Utils::getQueueCanonicalUri(queueUrl);
	const_cast<SQS*>(this)->_queueMap[queueName] = canonicalUri;
	return std::move(canonicalUri);
}


#include <iostream>

int main() {
	std::string secretKey = "XXX";
	std::string secretID  = "XXX";
	std::string region    = "eu-west-1";
	SQS sqs(secretKey, secretID, region);
	std::cout  << sqs.getQueue("queue-test") << std::endl;
}