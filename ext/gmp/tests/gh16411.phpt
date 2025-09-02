--TEST--
GH-16411 (gmp_export() can cause overflow)
--EXTENSIONS--
gmp
--FILE--
<?php
gmp_export("-9223372036854775808", PHP_INT_MAX, PHP_INT_MIN);
?>
--EXPECTF--
Fatal error: Uncaught ValueError: gmp_export(): Argument #2 ($word_size) is too large for argument #1 ($num) in %s:%d
%A
