--TEST--
GH-16411 (gmp_export() can cause overflow)
--EXTENSIONS--
gmp
--FILE--
<?php
gmp_export(-9223372036854775808, 9223372036854775807, -9223372036854775808);
?>
--EXPECTF--
Fatal error: Uncaught ValueError: gmp_export(): Argument #2 ($word_size) is too large for argument #1 ($num) in %s:%d
%A
