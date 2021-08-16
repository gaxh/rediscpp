#include "rediscpp_async.h"
#include "rediscpp_command.h"
#include "rediscpp_reply.h"
#include <sstream>
#include <unistd.h>
#include <signal.h>

#define RESULT(fmt, args...) printf("[%s:%d:%s]" fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)

static bool running = true;

int main() {
    RESULT("test begin");

    signal(SIGINT, [](int signum){running = false;});

    RediscppAsync redis;

    struct timeval timeout = {0};
    timeout.tv_sec = 2;

    redis.Connect("127.0.0.1", 6379, [&redis](int code){
                RESULT("redis connected: %d", code);

                RediscppCommand cmd;
                cmd.Arg("set").Arg("shit").Arg("123zzz");

                RESULT("execute command: %s", cmd.Dump().c_str());

                redis.Command(&cmd, [&redis](int code, const redisReply *reply){
                            if(code) {
                                RESULT("execute failed: %d", code);
                                return;
                            }

                            std::string result;
                            if(rediscpp_parse_reply(reply, &result)) {
                                RESULT("execute result: %s", result.c_str());

                                RediscppCommand cmd;
                                cmd.Arg("get").Arg("shit");

                                RESULT("execute command: %s", cmd.Dump().c_str());

                                redis.Command(&cmd, [](int code, const redisReply *reply) {
                                    if(code) {
                                        RESULT("execute failed: %d", code);
                                        return;
                                    }

                                    std::string result;
                                    if(rediscpp_parse_reply(reply, &result)) {
                                        RESULT("execute result: %s", result.c_str());

                                        running = false;
                                    }
                                });
                            }
                        });

            }, [](int code){
                RESULT("redis disconnected: %d", code);
            }, &timeout, &timeout);

    while(running) {
        usleep(50000);
        redis.HardUpdate();
    }

    redis.Disconnect();
    RESULT("test end");
    return 0;
}
