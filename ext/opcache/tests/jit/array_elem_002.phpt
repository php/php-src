--TEST--
Occupied next element
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
$float = 100000000000000000000000000000000000000;
$string_float= PHP_INT_MAX;
$a = [$float => 'a', $string_float => 'b', 'c', 'd'];
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot add element to the array as the next element is already occupied in %sarray_elem_002.php:4
Stack trace:
#0 {main}
  thrown in %sarray_elem_002.php on line 4
