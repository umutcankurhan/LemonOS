#include <objects/interface.h>

#include <string.h>
#include <scheduler.h>

MessageInterface::MessageInterface(const char* _name, uint16_t msgSize){
    name = strdup(_name);

    if(msgSize > MessageEndpoint::maxMessageSizeLimit){
        msgSize = MessageEndpoint::maxMessageSizeLimit;
    }

    this->msgSize = msgSize;
}

long MessageInterface::Accept(FancyRefPtr<MessageEndpoint>& endpoint){
    acquireLock(&incomingLock);
    if(incoming.get_length()){
        auto connection = incoming.remove_at(0);
        releaseLock(&incomingLock);

        auto channel = MessageEndpoint::CreatePair(msgSize);
        connection->item2 = channel.item2;
        connection->item1 = true;

        endpoint = connection->item2; 
        return 0;
    } else {
        releaseLock(&incomingLock);
        return 1;
    }
}

FancyRefPtr<MessageEndpoint> MessageInterface::Connect(){
    Pair<bool, FancyRefPtr<MessageEndpoint>> connection;

    connection.item1 = false;

    acquireLock(&incomingLock);
    incoming.add_back(&connection);
    releaseLock(&incomingLock);

    while(!connection.item1){
        Scheduler::Yield();
    }
    return connection.item2;
}