#
#	Win32 Makefile to run the PHP unit tests.
#
#	TEST_PHP_EXECUTABLE 
#		Required - must point to the PHP executable to test.
#
#	TEST_PHP_ERROR_STYLE 
#		Optional - specifies error format to output so IDE can jump to test source and log.
#		Values:  MSVC (Microsoft Visual C++), Emacs
#
#	TEST_PHP_DETAILED
#		Optional - generates a more complete and detailed log if set.
#		Values:  0 or unset - no details; 1 - detailed.
#

all : run-tests

BIN=Debug_TS

# Specific test(s) to run (all if not specified).
TESTS=

run-tests : 
	set TEST_PHP_EXECUTABLE=$(BIN)\php-cgi.exe
	set TEST_PHP_ERROR_STYLE=MSVC
	set TEST_PHP_DETAILED=0
	cd .. && $(BIN)\php-cgi.exe -c tests -f run-tests.php $(TESTS) | tee tests.log
