#ifndef __REDISCPP_SYNC_H__
#define __REDISCPP_SYNC_H__

#include <memory>
#include "rediscpp_reply.h"

class RediscppCommand;

struct redisContext;
struct redisReply;

class RediscppSync {
public:
    bool Connect( const char *ip, int port, struct timeval *connect_timeout = NULL, struct timeval *command_timeout = NULL);

    bool Reconnect();

    void Disconnect();

    std::shared_ptr<redisReply> Command( const RediscppCommand *cmd );

    template<typename ResultType>
    bool Command( const RediscppCommand *cmd, ResultType *result ) {
        std::shared_ptr<redisReply> reply = Command(cmd);
        return rediscpp_parse_reply(reply.get(), result);
    }

private:
    void AssignContext(redisContext *ctx);
    std::shared_ptr<redisContext> redis_ctx;
};

#endif

