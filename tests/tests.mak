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

all : run-tests

BIN=Debug_TS

run-tests :
	set TEST_PHP_EXECUTABLE=$(BIN)\php-cgi.exe
	set TEST_PHP_ERROR_STYLE=MSVC
	cd .. && $(BIN)\php-cgi.exe -c tests -f run-tests.php | tee tests.log
