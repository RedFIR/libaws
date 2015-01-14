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


SQSQueue::SQSQueue(const SQS *sqs, const std::string &queueUrl, const std::string & canonicalUri, int visibility) : _sqs(sqs), _queueUrl(queueUrl), _canonicalUri(canonicalUri), _visibility(visibility)
{}

SQS::SQS(const std::string & awsKeyID, const std::string& awsSecretKey, const std::string &region){
	this->_awsKeyID = awsKeyID;
	this->_awsSecretKey = awsSecretKey;
	this->_method = "GET";
	this->_service = "sqs";
	this->_region = region;
	this->_host = "sqs." + region + ".amazonaws.com";
	this->_endpoint = "https://" + this->_host; 
}

void SQSQueue::sendMessage(const std::string &message, const std::string &optionalParameter) const {
	std::string messageCpy(message);

	std::string param = "&MessageBody=" + Utils::escape(messageCpy) + optionalParameter;
	
	std::string requestUrl = this->_sqs->generateUrl(this->_canonicalUri, std::move("SendMessage"), param);

	std::stringstream ss;
	Utils::executeRequest(requestUrl, ss);
}

std::list<std::pair<std::string, std::string>> SQSQueue::recvMessages(int maxNumberOfMessages) const {
	assert(!(maxNumberOfMessages < 1 || maxNumberOfMessages > 10));

	std::string param = Utils::sprintf("&MaxNumberOfMessages=%&VisibilityTimeout=%", maxNumberOfMessages, this->_visibility);

	std::string requestUrl = this->_sqs->generateUrl(this->_canonicalUri, std::move("ReceiveMessage"), param);
	
	std::stringstream ss;
	Utils::executeRequest(requestUrl, ss);
		
	std::list<std::pair<std::string, std::string>> rep = Utils::getMessagesLst(ss);	
	return std::move(rep);
}

void SQSQueue::deleteMessage(std::string &receiptHandle) const {

	std::string param = "&ReceiptHandle=" + Utils::escape(receiptHandle);
	std::string requestUrl = this->_sqs->generateUrl(this->_canonicalUri, std::move("DeleteMessage"), param);

	std::stringstream ss;
	Utils::executeRequest(requestUrl, ss);
}

void SQSQueue::purge() const {
	std::string requestUrl = this->_sqs->generateUrl(this->_canonicalUri, std::move("PurgeQueue"));
}

const SQSQueue& SQS::getQueue(const std::string &queueName, bool create) const {
	auto it = this->_queueMap.find(queueName);
	if (it != this->_queueMap.end()) {
		return *(it->second);
	}

	if (create == false) {
		throw std::out_of_range("Can't find the queue named:" + queueName);
	}

	std::string escapedQueueName(queueName);
	std::string param = "&QueueName=" + Utils::escape(escapedQueueName);
	std::string requestUrl = this->generateUrl(std::move("/"), std::move("CreateQueue"), param);

	std::stringstream ss;
	Utils::executeRequest(requestUrl, ss);

	std::string queueUrl = Utils::getQueueUrl(ss);

	auto newQueue = std::shared_ptr<SQSQueue>(new SQSQueue(this, queueUrl,  Utils::getQueueCanonicalUri(queueUrl)));
	const_cast<SQS*>(this)->_queueMap[queueName] = newQueue;
	return *newQueue;
}

#include <iostream>

int main() {
	std::string secretKey = "XXX";
	std::string secretID  = "XXX";

	std::string region    = "eu-west-1";
	SQS sqs(secretID, secretKey, region);
	auto q =  sqs.getQueue("queue-test2");
	q.setVisibility(5);
	for (auto msg : q.recvMessages(3)) {
		std::cout << msg.first << std::endl;
		q.deleteMessage(msg.second);
	}
	q.purge();
}