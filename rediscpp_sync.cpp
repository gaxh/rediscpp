#include "rediscpp_sync.h"
#include <stdio.h>
#include "rediscpp_command.h"
#include "rediscpp_error.h"

extern "C" {
#include "hiredis.h"
}

static void RedisContextDeleter(redisContext *ctx) {
    rediscpp_error("free redis context");
    redisFree(ctx);
}

void RediscppSync::AssignContext(redisContext *ctx) {
    if(ctx) {
        redis_ctx.reset(ctx, RedisContextDeleter);
    } else {
        redis_ctx.reset();
    }
}

bool RediscppSync::Connect( const char *ip, int port, struct timeval *connect_timeout, struct timeval *command_timeout ) {
    redisContext *ctx = NULL;

    redisOptions option = {0};
    REDIS_OPTIONS_SET_TCP(&option, ip, port);
    option.connect_timeout = connect_timeout;
    option.command_timeout = command_timeout;

    ctx = redisConnectWithOptions(&option);

    if( !ctx || ctx->err ) {
        
        if(ctx) {
            rediscpp_error("connect redis %s:%d failed, errcode=%d", ip, port, ctx->err);
            redisFree(ctx);
        } else {
            rediscpp_error("connect redis %s:%d failed, no context alloced", ip, port);
        }

        return false;
    }

    rediscpp_error("connect redis %s:%d success", ip, port);
    AssignContext(ctx);

    return true;
}

static void RedisReplyDeleter(redisReply *reply) {
    freeReplyObject(reply);
}

std::shared_ptr<redisReply> RediscppSync::Command( const RediscppCommand *cmd ) {
    if(!redis_ctx) {
        rediscpp_error("redis context is NULL, can not execute command");
        return NULL;
    }

    redisReply *raw_p = (redisReply *)redisCommandArgv( redis_ctx.get(), cmd->GetArgc(), cmd->GetArgv(), cmd->GetArgvLen() );

    return raw_p ? std::shared_ptr<redisReply>( raw_p, RedisReplyDeleter ) : std::shared_ptr<redisReply>();
}

bool RediscppSync::Reconnect() {
    if(!redis_ctx) {
        rediscpp_error("redis context is NULL, can not reconnect");
        return false;
    }

    int errcode = redisReconnect(redis_ctx.get());

    if(errcode) {
        rediscpp_error("reconnect redis failed, errcode=%d", errcode);
        return false;
    }

    return true;
}

void RediscppSync::Disconnect() {
    AssignContext(NULL);
}


