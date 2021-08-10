#include "rediscpp_reply.h"
#include "hiredis.h"
#include "rediscpp_error.h"

bool rediscpp_parse_reply_do(const redisReply *reply, std::string *result) {
    switch(reply->type) {
        case REDIS_REPLY_NIL:
            result->clear();
            return true;
        case REDIS_REPLY_STRING:
        case REDIS_REPLY_STATUS:
            result->assign(reply->str, reply->len);
            return true;
        default:
            rediscpp_error("rediscpp_parse_reply_do error: type(%d) is not string or status", reply->type);
            return false;
    }
}

bool rediscpp_parse_reply_do(const redisReply *reply, long long *result) {
    switch(reply->type) {
        case REDIS_REPLY_NIL:
            *result = 0;
            return true;
        case REDIS_REPLY_INTEGER:
            *result = reply->integer;
            return true;
        case REDIS_REPLY_STRING:
            *result = strtoll(reply->str, NULL, 10);
            return true;
        default:
            rediscpp_error("rediscpp_parse_reply_do error: type(%d) is not integer", reply->type);
            return false;
    }
}


