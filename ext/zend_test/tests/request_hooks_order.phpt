--TEST--
Request hooks begin when userland code runs and end right before the module shutdown
--EXTENSIONS--
zend_test
--INI--
zend_test.print_request_hooks=1
--FILE--
<?php
echo "first request\n";
zend_test_request_boundary();
echo "next request\n";
?>
--EXPECT--
request_begin
first request
request_end
request_begin
next request
request_end
request_shutdown
