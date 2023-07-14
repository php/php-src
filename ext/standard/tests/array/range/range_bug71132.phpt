--TEST--
Bug #71132 (range function produces 2 segfaults with long integers)
--FILE--
<?php
var_dump(count(range(PHP_INT_MIN + 513, PHP_INT_MIN)));
var_dump(count(range(PHP_INT_MIN, PHP_INT_MIN + 513)));
?>
--EXPECT--
int(514)
int(514)
