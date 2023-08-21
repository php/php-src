--TEST--
Bug #71197 (range function produces another 2 segfaults with long integers)
--FILE--
<?php
range(PHP_INT_MIN, PHP_INT_MIN + 513, .01);
range(PHP_INT_MIN + 513, PHP_INT_MIN, .01);
?>
--EXPECT--
