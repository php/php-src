--TEST--
Stack limit 014 - Fuzzer
--SKIPIF--
<?php
if (!function_exists('zend_test_zend_call_stack_get')) die("skip zend_test_zend_call_stack_get() is not available");
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
?>
--EXTENSIONS--
zend_test
--INI--
; The test may use a large amount of memory on systems with a large stack limit
memory_limit=1G
--FILE--
<?php

try {
    require __DIR__.'/stack_limit_014.inc';
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
%S%rMaximum call stack size of [0-9]+ bytes \(zend\.max_allowed_stack_size - zend\.reserved_stack_size\) reached|Allowed memory size of [0-9]+ bytes exhausted%r%s
