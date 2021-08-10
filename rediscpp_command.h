#ifndef __REDISCPP_COMMAND_H__
#define __REDISCPP_COMMAND_H__

#include <string>
#include <type_traits>
#include <assert.h>
#include <list>

class RediscppCommand {
public:
    RediscppCommand &Arg(const char *s);

    RediscppCommand &Arg(const std::string &s);

    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value, int>::type = 1>
    RediscppCommand &Arg(T v){
        assert(argc < ARGC_MAX);

        std::string v_s = std::to_string(v);

        return Arg( *refs.insert( refs.end(), v_s ) );
    }
public:
    int GetArgc() const;

    const char **GetArgv() const;

    const size_t *GetArgvLen() const;

    void Clear();

    std::string Dump() const;

private:
    static const int ARGC_MAX = 8;
    const char *argv[ARGC_MAX];
    size_t argv_len[ARGC_MAX];
    int argc = 0;
    std::list<std::string> refs;
};


#endif
