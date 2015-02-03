#include <cstdlib>
#include <iostream>
#include "SQS.hpp"

using namespace LIBAWS;

int main() {
	std::string secretID;
	std::string secretKey;
	try {
		secretID  = std::string(std::getenv("AWS_SECRET_KEY_ID"));
		secretKey = std::string(std::getenv("AWS_SECRET_ACCESS_KEY"));
	}
	catch (const std::logic_error &e) {
		std::cerr << "Error AWS_SECRET_ACCESS_KEY or AWS_SECRET_KEY_ID missing in the environment" << std::endl;
		return (0);
	}

	std::string region    = "eu-west-1";
	SQS sqs(secretID, secretKey, region);
	auto q =  sqs.getQueue("queue-test2");
	q.setVisibility(30);
	std::vector<std::string> batchMessages;
	for (int i = 0; i < 45; i++){
		batchMessages.push_back("Message number" + std::to_string(i));
	}
	q.sendMessageBatch(batchMessages);
	batchMessages.clear();
	std::cout << q.size() << std::endl;
	while (q.size() > 10) {
		for (auto msg : q.recvMessages(10)) {
			std::cout << msg.first << std::endl;
			batchMessages.push_back(msg.second);
		}
		q.deleteMessageBatch(batchMessages);
	}
	q.purge();

	return (1);
}