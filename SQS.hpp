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
#include <algorithm> // replace

namespace LIBAWS {
	class	SQS {
		std::string _awsKeyID;
		std::string _awsSecretKey;
		std::string _method;
		std::string _service;
		std::string _host;
		std::string _region;
		std::string _endpoint;
		std::map<std::string, std::string> _queueMap;

	public:
		SQS(const std::string& awsSecretKey, const std::string& awsSecretID, const std::string &region);
		const std::string &getQueue(const std::string &queueName, bool create = true) const;
		void sendMessage(const std::string &queueUri, const std::string &message) const;
	};
}


#endif