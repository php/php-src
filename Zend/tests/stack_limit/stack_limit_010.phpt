--TEST--
Stack limit 010 - Check stack size detection against known defaults
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
if (!function_exists('zend_test_zend_call_stack_get')) die("skip zend_test_zend_call_stack_get() is not available");
if (!getenv('STACK_LIMIT_DEFAULTS_CHECK')) { die('skip STACK_LIMIT_DEFAULTS_CHECK not set'); }
?>
--FILE--
<?php

$stack = zend_test_zend_call_stack_get();

var_dump($stack);

$expectedMaxSize = match(php_uname('s')) {
    'Darwin' => 8*1024*1024,
    'FreeBSD' => match(php_uname('m')) {
        'amd64' => 512*1024*1024 - 4096,
        'i386' => 64*1024*1024 - 4096,
    },
    'Linux' => match (getenv('CI')) {
        'true' => 16*1024*1024, // https://github.com/actions/runner-images/pull/3328
        default => 8*1024*1024,
    },
    'Windows NT' => 67108864 - 4*4096, // Set by sapi/cli/config.w32
};

printf("Expected max_size: 0x%x\n", $expectedMaxSize);
printf("Actual   max_size: %s\n", $stack['max_size']);

var_dump($stack['max_size'] === sprintf('0x%x', $expectedMaxSize));

?>
--EXPECTF--
array(4) {
  ["base"]=>
  string(%d) "0x%x"
  ["max_size"]=>
  string(%d) "0x%x"
  ["position"]=>
  string(%d) "0x%x"
  ["EG(stack_limit)"]=>
  string(%d) "0x%x"
}
Expected max_size: 0x%x
Actual   max_size: 0x%x
bool(true)
