--TEST--
Request boundary releases the copies of the request input kept by ext/filter
--EXTENSIONS--
zend_test
filter
--ENV--
ZEND_TEST_FILTER=kept
--INI--
variables_order=EGPCS
--FILE--
<?php
$_SERVER;
zend_test_request_boundary();
$_SERVER;

$start = memory_get_usage();
for ($i = 0; $i < 100; $i++) {
    zend_test_request_boundary();
    $_SERVER;
}
var_dump(memory_get_usage() - $start < 100 * 64);
var_dump(filter_input(INPUT_ENV, 'ZEND_TEST_FILTER'));
?>
--EXPECT--
bool(true)
string(4) "kept"
