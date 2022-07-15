--TEST--
Stack overflow 010 - Check stack size detection against known defaults
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
if (!getenv('STACK_LIMIT_DEFAULTS_CHECK')) { die('skip STACK_LIMIT_DEFAULTS_CHECK not set'); }
?>
--FILE--
<?php

$stack = zend_test_zend_call_stack_get();

var_dump($stack);

[$expectedMaxUseableSize, $expectedGuardSize] = match(php_uname('s')) {
    'Darwin' => [8*1024*1024, 4096],
    'FreeBSD' => match(php_uname('m')) {
        'amd64' => [512*1024*1024 - 4096, 4096],
        'i386' => [64*1024*1024 - 4096, 4096],
    },
    'Linux' => [8*1024*1024, 256*4096],
    'Windows' => [67108864 - 4*4096, 4*4096], // Set by sapi/cli/config.w32
};

printf("Expected max_useable_size: 0x%x\n", $expectedMaxUseableSize);
printf("Expected max_size:         0x%x\n", $expectedMaxUseableSize + $expectedGuardSize);

var_dump($stack['max_useable_size'] === sprintf('0x%x', $expectedMaxUseableSize,));
var_dump($stack['max_size'] === sprintf('0x%x', $expectedMaxUseableSize + $expectedGuardSize));

?>
--EXPECTF--
array(4) {
  ["base"]=>
  string(%d) "0x%x"
  ["max_useable_size"]=>
  string(%d) "0x%x"
  ["max_size"]=>
  string(%d) "0x%x"
  ["position"]=>
  string(%d) "0x%x"
}
Expected max_useable_size: 0x%x
Expected max_size:         0x%x
bool(true)
bool(true)
