#include "rediscpp_command.h"
#include <string.h>
#include <assert.h>
#include <sstream>

RediscppCommand &RediscppCommand::Arg(const char *s) {
    assert(argc < ARGC_MAX);

    argv[argc] = s;
    argv_len[argc++] = strlen(s);

    return *this;
}

RediscppCommand &RediscppCommand::Arg(const std::string &s) {
    assert(argc < ARGC_MAX);

    argv[argc] = s.c_str();
    argv_len[argc++] = s.size();

    return *this;
}

int RediscppCommand::GetArgc() const {
    return argc;
}

const char **RediscppCommand::GetArgv() const {
    return (const char **)argv;
}

const size_t *RediscppCommand::GetArgvLen() const {
    return argv_len;
}

void RediscppCommand::Clear() {
    argc = 0;
}

std::string RediscppCommand::Dump() const {
    std::ostringstream ss;

    for(int i = 0; i < argc; ++i) {
        ss << std::string( argv[i], argv_len[i] ) << " ";
    }

    return ss.str();
}
