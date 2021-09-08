
HIREDIS_INC = 3rd/hiredis/
HIREDIS_STATICLIB = 3rd/hiredis/libhiredis.a

CFLAGS = -g -Wall -I$(HIREDIS_INC)
CFILES = rediscpp_sync.cpp rediscpp_command.cpp rediscpp_reply.cpp rediscpp_async.cpp
CHEADERS = rediscpp_sync.h rediscpp_command.h rediscpp_reply.h rediscpp_error.h rediscpp_async.h rediscpp_context.h

all: rediscpp_sync_test

all: rediscpp_async_test

rediscpp_sync_test: $(HIREDIS_STATICLIB) $(CFILES) $(CHEADERS) rediscpp_sync_test.cpp
	$(CXX) $(CFLAGS) -o $@ $(CFILES) rediscpp_sync_test.cpp $(LDFLAGS) $(HIREDIS_STATICLIB)

rediscpp_async_test: $(HIREDIS_STATICLIB) $(CFILES) $(CHEADERS) rediscpp_async_test.cpp
	$(CXX) $(CFLAGS) -o $@ $(CFILES) rediscpp_async_test.cpp $(LDFLAGS) $(HIREDIS_STATICLIB)

$(HIREDIS_STATICLIB): 3rd/hiredis/Makefile
	cd 3rd/hiredis/ && $(MAKE)

clean:
	cd 3rd/hiredis/ && $(MAKE) clean
	rm -f rediscpp_sync_test
	rm -f rediscpp_async_test


