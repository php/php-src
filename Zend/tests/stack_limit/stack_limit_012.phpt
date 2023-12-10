--TEST--
Stack limit 012 - Stack limit exhaustion during unwinding
--SKIPIF--
<?php
if (!function_exists('zend_test_zend_call_stack_get')) die("skip zend_test_zend_call_stack_get() is not available");
?>
--EXTENSIONS--
zend_test
--INI--
zend.max_allowed_stack_size=256K
--FILE--
<?php

var_dump(zend_test_zend_call_stack_get());

function replace() {
    return preg_replace_callback('#.#', function () {
        try {
            replace();
        } finally {
            require __DIR__ . '/stack_limit_012.inc';
        }
    }, 'x');
}

replace();

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

Fatal error: Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached during compilation. Try splitting expression in %s on line %d
