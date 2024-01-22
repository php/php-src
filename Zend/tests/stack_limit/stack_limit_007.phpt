--TEST--
Stack limit 007 - Exception handling
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
            $tryExecuted = true;
            return replace();
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
            // We should not enter the catch block if we haven't executed at
            // least one op in the try block
            printf("Try executed: %d\n", $tryExecuted ?? 0);
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
Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
Try executed: 1
