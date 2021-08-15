#ifndef __REDISCPP_ASYNC_H__
#define __REDISCPP_ASYNC_H__

#include <memory>
#include <functional>
#include <map>

struct redisAsyncContext;
struct redisReply;

class RediscppAsync {
public:
    using connect_cb_type = std::function<void(int code)>;
    using disconnect_cb_type = std::function<void(int code)>;

    bool Connect( const char *ip, int port, connect_cb_type connect_cb, disconnect_cb_type disconnect_cb, struct timeval *connect_timeout = NULL, struct timeval *command_timeout = NULL );

    void Disconnect();

    void Command();

    void HardUpdate();

private:
    void AssignContext(redisAsyncContext *ctx);
    std::shared_ptr<redisAsyncContext> redis_ctx;

    connect_cb_type m_connect_cb;
    disconnect_cb_type m_disconnect_cb;

    // 回调里面用
    static void RedisConnectCallback(const redisAsyncContext *ctx, int status);
    static void RedisDisconnectCallback(const redisAsyncContext *ctx, int status);
    static void RedisAsyncContextDeleter(redisAsyncContext *ctx);
};

#endif
