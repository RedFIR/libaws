LibAWS
=======

An object-oriented C++(11) wrapper for AWS (currently only SQS)

If you want to know a bit more about AWS, you should go on the official website and read about the api: http://aws.amazon.com/fr/documentation/

Compile and link
================

Standalone
----------
On ubuntu (14.04)
```
sudo apt-get install libcurl4-openssl-dev libxml++2.6-dev libcrypto++-dev
make
make demo
EXPORT AWS_SECRET_KEY_ID=""
EXPORT AWS_SECRET_ACCESS_KEY=""
./demo
```

```
g++ example/example1.cpp lib/lib*.a -Iheaders/ -std=c++0x -lcurl -std=c++0x `pkg-config libxml++-2.6 --cflags --libs` -lcryptopp -o demo
```

Example
================

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