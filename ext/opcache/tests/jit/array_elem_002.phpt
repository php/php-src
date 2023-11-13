--TEST--
Occupied next element
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function foo() {
    $float = 100000000000000000000000000000000000000;
    $string_float= PHP_INT_MAX;
    $a = [$float => 'a', $string_float => 'b', 'c', 'd'];
}
foo();
?>
DONE
--EXPECTF--
Deprecated: Implicit conversion from float 1.0E+38 to int loses precision in %sarray_elem_002.php on line 5

Fatal error: Uncaught Error: Cannot add element to the array as the next element is already occupied in %sarray_elem_002.php:5
Stack trace:
#0 %sarray_elem_002.php(7): foo()
#1 {main}
  thrown in %sarray_elem_002.php on line 5
