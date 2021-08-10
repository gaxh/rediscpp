#include "rediscpp_sync.h"
#include <stdio.h>
#include "rediscpp_command.h"
#include "rediscpp_error.h"

extern "C" {
#include "hiredis.h"
}

int RediscppSync::LastErrorCode() const {
    return redis_ctx ? redis_ctx->err : 0;
}

const char *RediscppSync::LastErrorMessage() const {
    return redis_ctx ? redis_ctx->errstr : "";
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

bool RediscppSync::Connect( const char *ip, int port, const struct timeval *timeout ) {
    redisContext *ctx = NULL;

    if( timeout ) {
        ctx = redisConnectWithTimeout(ip, port, *timeout);
    } else {
        ctx = redisConnect(ip, port);
    }

    if( !ctx || ctx->err ) {
        
        if(ctx) {
            rediscpp_error("connect redis %s:%d failed, errcode=%d", ip, port, ctx->err);
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

