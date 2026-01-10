--TEST--
Attempted read/write of backing value in sibling property hook fails
--SKIPIF--
<?php
if (!function_exists('zend_test_zend_call_stack_get')) die("skip zend_test_zend_call_stack_get() is not available");
?>
--EXTENSIONS--
zend_test
--INI--
; The test may use a large amount of memory on systems with a large stack limit
memory_limit=2G
--FILE--
<?php

class Test {
    public $a = 1 {
        get => $this->a + $this->b;
    }
    public $b = 2 {
        get => $this->b + $this->a;
    }
}

$test = new Test;

try {
    var_dump($test->a);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
