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
#ifndef __LIB_AWS_SQS__HPP__
#define __LIB_AWS_SQS__HPP__

#include <ctime>
#include <string>
#include <sstream>
#include <map>
#include <list>
#include <utility>
#include <algorithm> // replace
#include <memory>
#include "Utils.hpp"

namespace LIBAWS {
	class SQS;

	class SQSQueue{
		const SQS*  _sqs;
		std::string _queueUrl;
		std::string _canonicalUri;
		int			_visibility;
	public:
		friend SQS;
		SQSQueue(const SQS *sqs, const std::string &queueUrl, const std::string & canonicalUri, int visibility = 300);
		void setVisibility(int visibility) { _visibility = visibility; }
		int  getVisibility() const { return _visibility; }

		void sendMessage(const std::string &message, const std::string &optionalParameter = "") const;
		std::list<std::pair<std::string, std::string>> recvMessages(int maxNumberOfMessages) const;
		void deleteMessage(std::string &receiptHandle) const;
		void purge() const;
		int  size() const;

		// Will throw an exception if the sum of all a batch's (10maximum) individual message lengths) is above 256kb
		void sendMessageBatch(std::vector<std::string> &messageList) const;
		void deleteMessageBatch(std::vector<std::string> &messageList) const;

	};


	class	SQS : public Utils::AWSAuth {
		std::map<std::string, std::shared_ptr<SQSQueue>>    _queueMap;

	public:
		SQS(const std::string& awsSecretKey, const std::string& awsSecretID, const std::string &region);
		const SQSQueue& getQueue(const std::string &queueName, bool create = true) const;
		//void deleteMessage(const std::string &canonicalUri, std::string &receiptHandle) const;
	};
}


#endif