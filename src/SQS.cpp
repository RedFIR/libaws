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
#include <cassert>

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
	if (param.size() > (262000 + 13 + optionalParameter.size())) {
		throw (std::runtime_error("The maximum allowed individual message size is 256 KB"));
	}
	
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
	
	std::stringstream ss;
	Utils::executeRequest(requestUrl, ss);
}


int  SQSQueue::size() const {
	std::string param = "&Action=GetQueueAttributes&AttributeName.1=ApproximateNumberOfMessages";
	std::string requestUrl = this->_sqs->generateUrl(this->_canonicalUri, std::move("GetQueueAttributes"), param);

	std::stringstream ss;
	Utils::executeRequest(requestUrl, ss);
	return Utils::getQueueSize(ss);	
}

void SQSQueue::sendMessageBatch(std::vector<std::string> &messageList) const {
	std::stringstream ss;
	std::string requestUrl; 
	std::string param;
	unsigned int j;

	for (unsigned int i = 0; i < messageList.size();) {
		param = "";
		unsigned totalSize = 0;
		for (j = 0; j < 9 && j + i < messageList.size();j++){
			std::string msg = messageList[i + j];
			msg = Utils::escape(msg);
			totalSize += msg.size();
			param += Utils::sprintf("&SendMessageBatchRequestEntry.%.Id=msg_00%&SendMessageBatchRequestEntry.%.MessageBody=%", j + 1, j + 1, j + 1, msg);
		}
		i += j;
		if (totalSize > 262000) { //real value is 262,144 bytes but let's not push it to the limit
			throw (std::runtime_error("The sum of all a batch's individual message lengths is above 256 KB"));
		}
		requestUrl = this->_sqs->generateUrl(this->_canonicalUri, std::move("SendMessageBatch"), param);
		Utils::executeRequest(requestUrl, ss);
	}

}

void SQSQueue::deleteMessageBatch(std::vector<std::string> &messageList) const {
	std::stringstream ss;
	std::string requestUrl;
	std::string param;
	unsigned int j;

	for (unsigned int i = 0; i < messageList.size();) {
		param = "";
		unsigned totalSize = 0;
		for (j = 0; j < 9 && j + i < messageList.size();j++){
			std::string msg = messageList[i + j];
			msg = Utils::escape(msg);
			totalSize += msg.size();
			param += Utils::sprintf("&DeleteMessageBatchRequestEntry.%.Id=msg%&DeleteMessageBatchRequestEntry.%.ReceiptHandle=%", j + 1, j + 1, j + 1, msg);
		}
		i += j;
		if (totalSize > 262000) { //real value is 262,144 bytes but let's not push it to the limit
			throw (std::runtime_error("The sum of all a batch's individual message lengths is above 256 KB"));
		}
		requestUrl = this->_sqs->generateUrl(this->_canonicalUri, std::move("DeleteMessageBatch"), param);
		Utils::executeRequest(requestUrl, ss);
	}

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

// int main() {
// 	std::string secretKey = "XXX";
// 	std::string secretID  = "XXX";

// 	std::string region    = "eu-west-1";
// 	SQS sqs(secretID, secretKey, region);
// 	auto q =  sqs.getQueue("queue-test2");
// 	q.setVisibility(5);
// 	std::vector<std::string> batchMessages;

// 	for (int i = 0; i < 45; i++){
// 		batchMessages.push_back("Message number" + std::to_string(i));
// 	}
// 	q.sendMessageBatch(batchMessages);
// 	batchMessages.clear();
// 	std::cout << q.size() << std::endl;
// 	while (q.size() > 10) {
// 		for (auto msg : q.recvMessages(10)) {
// 			std::cout << msg.first << std::endl;
// 			batchMessages.push_back(msg.second);
// 		}
// 		q.deleteMessageBatch(batchMessages);
// 	}
// 	q.purge();
// }