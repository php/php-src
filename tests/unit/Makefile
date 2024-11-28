CC = gcc
CFLAGS = -g -Wall -I../../ -I../../Zend -I../../main -I../../TSRM -I. -I..
COMMON_LDFLAGS = ../../.libs/libphp.a -lcmocka -lpthread -lm -ldl -lresolv -lutil

TESTS = main/test_network
main/test_network_SRC = main/test_network.c
main/test_network_LDFLAGS = $(COMMON_LDFLAGS) -Wl,--wrap=connect,--wrap=poll,--wrap=getsockopt,--wrap=gettimeofday


# Build all tests
all: $(TESTS)

# Build rule for each test
$(TESTS):
	$(CC) $(CFLAGS) -o $@.out $($(basename $@)_SRC) $($(basename $@)_LDFLAGS)

# Run all tests
.PHONY: test
test: $(TESTS)
	@echo "Running all tests..."
	@for test in $(TESTS); do \
		echo "Running $$test..."; \
		$$test.out || exit 1; \
	done

# Clean tests
.PHONY: clean
clean:
	@for test in $(TESTS); do \
		rm -f $$test.out; \
	done
