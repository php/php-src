--TEST--
GHSA-gr4f-wgx5-jpfv: DATEG(tzcache) use-after-free
--CREDITS--
Sebastian Bergmann
--CGI--
--ENV--
QUERY_STRING=a=1&b=2
--EXTENSIONS--
session
zend_test
--INI--
error_log="{TMP}/GHSA-gr4f-wgx5-jpfv.log"
log_errors=1
max_input_vars=1
zend_test.warn_on_startup=1
--FILE--
<?php
echo "Hello world\n";
?>
--EXPECTF--
Warning: PHP Request Startup: Input variables exceeded 1. To increase the limit change max_input_vars in php.ini. in Unknown on line 0
Hello world%r(?:\n\nWarning: Test startup warning from zend_test in Unknown on line 0)?%r
