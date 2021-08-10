
HIREDIS_INC = 3rd/hiredis/
HIREDIS_STATICLIB = 3rd/hiredis/libhiredis.a

CFLAGS = -g -Wall -I$(HIREDIS_INC) --static
CFILES = rediscpp_sync_test.cpp rediscpp_sync.cpp rediscpp_command.cpp rediscpp_reply.cpp
CHEADERS = rediscpp_sync.h rediscpp_command.h rediscpp_reply.h rediscpp_error.h

all: rediscpp_sync_test

rediscpp_sync_test: $(HIREDIS_STATICLIB) $(CFILES) $(CHEADERS) 
	$(CXX) $(CFLAGS) -o $@ $(CFILES) $(LDFLAGS) $(HIREDIS_STATICLIB)

$(HIREDIS_STATICLIB): 3rd/hiredis/Makefile
	cd 3rd/hiredis/ && $(MAKE)

clean:
	cd 3rd/hiredis/ && $(MAKE) clean
	rm -f rediscpp_sync_test

