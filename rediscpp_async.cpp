#include "rediscpp_async.h"
#include <stdio.h>
#include "rediscpp_command.h"
#include "rediscpp_error.h"

extern "C" {
#include "hiredis.h"
#include "async.h"
}

void RediscppAsync::RedisAsyncContextDeleter(redisAsyncContext *ctx) {
    RediscppAsync *this_ptr = (RediscppAsync *)ctx->c.privdata;

    if(this_ptr->IsDisconnecting()) {
        rediscpp_error("free redis async context, do not free");
    } else {
        rediscpp_error("free redis async context");
        redisAsyncFree(ctx);
    }
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

    ++this_ptr->m_disconnecting;
    
    this_ptr->AssignContext(NULL);

    --this_ptr->m_disconnecting;
}

void RediscppAsync::Command(const RediscppCommand *cmd, command_cb_type cb) {
    if(!redis_ctx) {
        rediscpp_error("redis context is NULL, can not execute command");
        if(cb) {cb(-1, NULL);}
        return;
    }

    void *ud = NULL;

    if(cb) {
        ud = m_contexts.Push(std::move(cb));
    }

    int code = redisAsyncCommandArgv(redis_ctx.get(), RediscppAsync::RedisCommandCallback, ud, cmd->GetArgc(), cmd->GetArgv(), cmd->GetArgvLen());

    if(code){
        rediscpp_error("redis async command failed: %d", code);
    }

    if(code && ud) {
        // 执行失败
        rediscpp_error("redis async command failed with ud: %d", code);
        command_cb_type callback;

        if(m_contexts.Pop(ud, &callback)) {
            callback(code, NULL);
        }
    }
}

void RediscppAsync::RedisCommandCallback(redisAsyncContext *ctx, void *r, void *ud) {
    RediscppAsync *this_ptr = (RediscppAsync *)ctx->c.privdata;

    if(!this_ptr) {
        rediscpp_error("can not get RediscppAsync instance");
        return;
    }

    redisReply *reply = (redisReply *)r;

    if(ud) {
        command_cb_type callback;

        if(this_ptr->m_contexts.Pop(ud, &callback)) {
            callback(reply ? 0 : -2, reply);
        }
    }
}

void RediscppAsync::HardUpdate() {
    if(redis_ctx) redisAsyncHandleRead(redis_ctx.get());
    if(redis_ctx) redisAsyncHandleWrite(redis_ctx.get());
//    if(redis_ctx) redisAsyncHandleTimeout(redis_ctx.get());
}

void RediscppAsync::Disconnect() {
    if(redis_ctx) {
        redisAsyncDisconnect(redis_ctx.get());
    }
}

