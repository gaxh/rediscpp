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
    timeout.tv_sec = 1;

    redis.Connect("127.0.0.1", 6379, [](int code){
                RESULT("redis connected: %d", code);
            }, [](int code){
                RESULT("redis disconnected: %d", code);
            }, &timeout, &timeout);

    while(running) {
        usleep(10000);
        redis.HardUpdate();
    }

    RESULT("test end");
    return 0;
}
