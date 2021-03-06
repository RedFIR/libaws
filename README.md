LibAWS
=======

An object-oriented C++(11) wrapper for AWS (currently only SQS)

If you want to know a bit more about AWS, you should go on the official website and read about the api: http://aws.amazon.com/fr/documentation/

Compile and link
================

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
=======

`````c++
#include <cstdlib>
#include "SQS.hpp"


int main() {
    std::string secretKey = std::getenv("AWS_SECRET_ACCESS_KEY");
    std::string secretID  = std::getenv("AWS_SECRET_KEY_ID");
    std::string region    = "eu-west-1";

    SQS sqs(secretID, secretKey, region);
    auto q =  sqs.getQueue("queue-test2");
    q.setVisibility(5); // set the visibility of the messages (default 300)

    std::vector<std::string> batchMessages;
    // create 45 messages
    for (int i = 0; i < 45; i++){
        batchMessages.push_back("Message number" + std::to_string(i));
    }

    //push the 45 messages in the queue
    q.sendMessageBatch(batchMessages);

    batchMessages.clear();
    //print the size of the queue
    std::cout << q.size() << std::endl;

    while (q.size() > 10) {
        // try to read 10 messages or less
        for (auto msg : q.recvMessages(10)) {
            std::cout << msg.first << std::endl;
            batchMessages.push_back(msg.second);
        }
        // delete all the 10 or less messages
        q.deleteMessageBatch(batchMessages);
    }

    // purge the queue
    q.purge();
}
`````

Warning
=======
The error handling is very limited (especially for the aws responses)
Howewer you can compile with `-DDEBUG` in order to see the url and try them in your browser