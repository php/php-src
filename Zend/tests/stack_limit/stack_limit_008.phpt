--TEST--
Stack limit 008 - Exception handling
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
            return replace();
        } catch (Error $e) {
            echo "Will throw:\n";
            try {
                return replace2();
            } catch (Error $e) {
                echo $e->getMessage();
            }
        }
    }, 'x');
}

function replace2() {
    return preg_replace_callback('#.#', function () {
        try {
            return '';
        } finally {
            // We should not enter the finally block if we haven't executed at
            // least one op in the try block
            echo "Finally block: This should not execute\n";
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
Will throw:
Maximum call stack size of %d bytes (zend.max_allowed_stack_size-zend.reserved_stack_size) reached. Infinite recursion?
