you need libcurl libcrypto++ libxml libxml++  (on ubuntu you have all the libs in the repository)
make & make demo
./demo


`````c++
#include <cstdlib>
#include "SQS.hpp"
int main() {
    std::string secretKey = std::getenv("AWS_SECRET_ACCESS_KEY");
    std::string secretID  = std::getenv("AWS_SECRET_KEY_ID");


    std::string region    = "eu-west-1";
    SQS sqs(secretID, secretKey, region);
    auto q =  sqs.getQueue("queue-test2");
    q.setVisibility(5);
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
}
`````