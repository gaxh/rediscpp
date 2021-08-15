#include "rediscpp_async.h"
#include <stdio.h>
#include "rediscpp_command.h"
#include "rediscpp_error.h"

extern "C" {
#include "hiredis.h"
#include "async.h"
}

void RediscppAsync::RedisAsyncContextDeleter(redisAsyncContext *ctx) {
    rediscpp_error("free redis async context");
    redisAsyncFree(ctx);
}

void RediscppAsync::AssignContext(redisAsyncContext *ctx) {
    if(ctx) {
        redis_ctx.reset(ctx, RediscppAsync::RedisAsyncContextDeleter);
    } else {
        redis_ctx.reset();
    }
}

bool RediscppAsync::Connect( const char *ip, int port, connect_cb_type connect_cb, disconnect_cb_type disconnect_cb, struct timeval *connect_timeout, struct timeval *command_timeout ) {
    redisAsyncContext *ctx = NULL;

    redisOptions option = {0};
    REDIS_OPTIONS_SET_TCP(&option, ip, port);
    option.connect_timeout = connect_timeout;
    option.command_timeout = command_timeout;
    option.privdata = this;

    ctx = redisAsyncConnectWithOptions(&option);

    if( !ctx || ctx->err ) {
        if(ctx) {
            rediscpp_error("connect redis %s:%d failed, errcode=%d", ip, port, ctx->err);
            redisAsyncFree(ctx);
        } else {
            rediscpp_error("connect redis %s:%d failed, no context alloced", ip, port);
        }
    }

    rediscpp_error("create redis %s:%d async context success", ip, port);
    AssignContext(ctx);

    m_connect_cb = connect_cb;
    m_disconnect_cb = disconnect_cb;

    redisAsyncSetConnectCallback(ctx, RediscppAsync::RedisConnectCallback);
    redisAsyncSetDisconnectCallback(ctx, RediscppAsync::RedisDisconnectCallback);

    return true;
}


void RediscppAsync::RedisConnectCallback(const redisAsyncContext *ctx, int status) {
    RediscppAsync *this_ptr = (RediscppAsync *)ctx->c.privdata;

    if(!this_ptr) {
        rediscpp_error("can not get RediscppAsync instance");
        return;
    }

    rediscpp_error("redis connect callback, code=%d", status);

    if(this_ptr->m_connect_cb) {
        this_ptr->m_connect_cb(status);
    }
}

void RediscppAsync::RedisDisconnectCallback(const redisAsyncContext *ctx, int status) {
    RediscppAsync *this_ptr = (RediscppAsync *)ctx->c.privdata;

    if(!this_ptr) {
        rediscpp_error("can not get RediscppAsync instance");
        return;
    }

    rediscpp_error("redis disconnect callback, code=%d", status);
    
    if(this_ptr->m_disconnect_cb) {
        this_ptr->m_disconnect_cb(status);
    }
}

void RediscppAsync::HardUpdate() {
    if(redis_ctx) {
        redisAsyncHandleRead(redis_ctx.get());
        redisAsyncHandleWrite(redis_ctx.get());
        redisAsyncHandleTimeout(redis_ctx.get());
    }    
}

