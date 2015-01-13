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
	std::string amzDate =  Utils::getAmzDate(t); 
	std::string datestamp = Utils::getDatestamp(t);

	std::string canonicalUri("/");
	std::string canonicalHeaders = "host:" + this->_host + "\n";
	std::string signedHeaders = "host";

	std::string algorithm = "AWS4-HMAC-SHA256";
	std::string credentialScope = datestamp + "/" + this->_region + "/" + this->_service + "/" + "aws4_request";

	std::string canonicalQuerystring = "Action=CreateQueue&QueueName=" + queueName; // TODO: verif queueName
	canonicalQuerystring += "&X-Amz-Algorithm=AWS4-HMAC-SHA256";
	std::string tmp = this->_awsKeyID + "/" + credentialScope;
	canonicalQuerystring += "&X-Amz-Credential=" + Utils::replace(tmp.begin(), tmp.end(), '/', "%2F"); //urllib.quote_plus
	canonicalQuerystring += "&X-Amz-Date=" + amzDate;
	canonicalQuerystring += "&X-Amz-Expires=30";
	canonicalQuerystring += "&X-Amz-SignedHeaders=" + signedHeaders;

	std::string payloadHash = Crypto::shaDigest();

	std::string canonicalRequest = this->_method + '\n' + canonicalUri + '\n' + canonicalQuerystring + '\n' + canonicalHeaders + '\n' + signedHeaders + '\n' + payloadHash;

	std::string stringToSign = algorithm + '\n' +  amzDate + '\n' +  credentialScope + '\n' +  Crypto::shaDigest(canonicalRequest);

	std::array<byte, 32> signingKey = Crypto::getSignatureKey(this->_awsSecretKey, datestamp, this->_region, this->_service);

	auto _sign = Crypto::sign(signingKey, stringToSign);
	std::string signature = Crypto::hexDump(_sign);
	std::transform(signature.begin(), signature.end(), signature.begin(), ::tolower);


	canonicalQuerystring += "&X-Amz-Signature=" + signature;
	std::string requestUrl = this->_endpoint + "?" + canonicalQuerystring;

	std::stringstream ss;
	Utils::executeRequest(requestUrl, ss);
	std::string queueUrl = Utils::getQueueUrl(ss);
	canonicalUri = Utils::getQueueCanonicalUri(queueUrl);
	const_cast<SQS*>(this)->_queueMap[queueName] = canonicalUri;
	return std::move(canonicalUri);
}

#include <iostream>

int main() {
	std::string secretKey = "XXX";
	std::string secretID  = "XXX";
	std::string region    = "eu-west-1";
	SQS sqs(secretKey, secretID, region);
	std::cout  << sqs.getQueue("queue-test2") << std::endl;
}