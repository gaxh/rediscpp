#ifndef __REDIS_REPLY_H__
#define __REDIS_REPLY_H__

#include <string>
#include"hiredis.h"
#include "rediscpp_error.h"
#include <vector>
#include <map>

struct redisReply;

template<typename ResultType>
bool rediscpp_parse_reply(const redisReply *reply, ResultType *result) {
    if(!reply) {
        rediscpp_error("rediscpp_parse_reply failed: redisReply is NULL");
        return false;
    }

    if(reply->type == REDIS_REPLY_ERROR) {
        rediscpp_error("rediscpp_parse_reply error: %s", reply->str);
        return false;
    }

    return rediscpp_parse_reply_do(reply, result);
}

template<typename ResultType>
bool rediscpp_parse_reply_do(const redisReply *reply, ResultType *result);

bool rediscpp_parse_reply_do(const redisReply *reply, std::string *result);

bool rediscpp_parse_reply_do(const redisReply *reply, long long *result);


template<typename ResultInnerType>
bool rediscpp_parse_reply_do(const redisReply *reply, std::vector<ResultInnerType> *result) {
    result->clear();
    switch(reply->type) {
        case REDIS_REPLY_NIL:
            return true;
        case REDIS_REPLY_ARRAY:
            result->resize( reply->elements );
            for(size_t i = 0; i < reply->elements; ++i) {
                redisReply *sub_reply = reply->element[i];

                if(!rediscpp_parse_reply_do(sub_reply, &result->at(i))) {
                    return false;
                }
            }
            return true;
        default:
            rediscpp_error("rediscpp_parse_reply_do error: type(%d) is not array", reply->type);
            return false;
    }
}

template<typename ResultKeyType, typename ResultValueType>
bool rediscpp_parse_reply_do(const redisReply *reply, std::map<ResultKeyType, ResultValueType> *result) {
    result->clear();
    switch(reply->type) {
        case REDIS_REPLY_NIL:
            return true;
        case REDIS_REPLY_ARRAY:
            {
                for(size_t i = 1; i < reply->elements; i += 2) {
                    redisReply *key_reply = reply->element[i - 1];
                    redisReply *value_reply = reply->element[i];

                    std::pair<ResultKeyType, ResultValueType> kvpair;

                    if(!rediscpp_parse_reply_do(key_reply, &kvpair.first)) {
                        return false;
                    }

                    if(!rediscpp_parse_reply_do(value_reply, &kvpair.second)) {
                        return false;
                    }

                    result->insert(std::move(kvpair));
                }
            }
            return true;
        default:
            rediscpp_error("rediscpp_parse_reply_do error: type(%d) is not array", reply->type);
            return false;
    }
}

#endif

