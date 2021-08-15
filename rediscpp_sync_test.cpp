#include "rediscpp_reply.h"
#include "rediscpp_sync.h"
#include "rediscpp_command.h"
#include <sstream>

#define RESULT(fmt, args...) printf("[%s:%d:%s]" fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)

static void execute_result_string(RediscppSync *redis, const RediscppCommand *cmd) {
    RESULT("execute command: %s", cmd->Dump().c_str());

    std::string result;
    bool ok = redis->Command(cmd, &result);
    RESULT("execute ok=%d, result=%s", ok, result.c_str());
}

static void execute_result_integer(RediscppSync *redis, const RediscppCommand *cmd) {
    RESULT("execute command: %s", cmd->Dump().c_str());

    long long result;
    bool ok = redis->Command(cmd, &result);
    RESULT("execute ok=%d, result=%lld", ok, result);
}

static void execute_result_vector(RediscppSync *redis, const RediscppCommand *cmd) {
    RESULT("execute command: %s", cmd->Dump().c_str());

    std::vector<std::string> result;
    bool ok = redis->Command(cmd, &result);

    std::ostringstream ss;
    for(size_t i = 0; i < result.size(); ++i) {
        ss << i << ":" << result[i] << ",";
    }

    RESULT("execute ok=%d, result_size=%zu, result=(%s)", ok, result.size(), ss.str().c_str());
}

static void execute_result_map(RediscppSync *redis, const RediscppCommand *cmd) {
    RESULT("execute command: %s", cmd->Dump().c_str());

    std::map<std::string, std::string> result;
    bool ok = redis->Command(cmd, &result);

    std::ostringstream ss;
    for(decltype(result)::iterator i = result.begin(); i != result.end(); ++i) {
        ss << i->first << ":" << i->second << ",";
    }

    RESULT("execute ok=%d, result_size=%zu, result=(%s)", ok, result.size(), ss.str().c_str());
}

int main(){
    RESULT("test begin");

    RediscppSync redis;

    struct timeval timeout = {0};
    timeout.tv_sec = 1;

    redis.Connect("127.0.0.1", 6379, &timeout, &timeout);

    RediscppCommand cmd;

    {
        cmd.Clear();

        cmd.Arg("auth").Arg(123456);

        execute_result_string(&redis, &cmd);
    }

    {
        cmd.Clear();

        cmd.Arg("set").Arg("shit").Arg("123zzz");

        execute_result_string(&redis, &cmd);
    }

    {
        cmd.Clear();

        cmd.Arg("get").Arg("shit");

        execute_result_string(&redis, &cmd);
    }

    for(int i = 0; i < 10; ++i) {
        cmd.Clear();

        cmd.Arg("lpush").Arg("shit1").Arg(i);

        execute_result_integer(&redis, &cmd);
    }

    {
        cmd.Clear();

        cmd.Arg("lrange").Arg("shit1").Arg(0).Arg(100);

        execute_result_vector(&redis, &cmd);
    }

    for(int i = 0; i < 10; ++i) {
        cmd.Clear();

        cmd.Arg("hset").Arg("shit2").Arg(std::string("KEY_") + std::to_string(i)).Arg(i);

        execute_result_integer(&redis, &cmd);
    }

    {
        cmd.Clear();

        cmd.Arg("hgetall").Arg("shit2");

        execute_result_map(&redis, &cmd);
    }

    {
        cmd.Clear();

        cmd.Arg("flushall");

        execute_result_string(&redis, &cmd);
    }

    redis.Reconnect();

    redis.Disconnect();

    RESULT("test end");

    return 0;
}


