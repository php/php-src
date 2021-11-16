--TEST--
substr() with PHP_INT_MIN offset or length
--FILE--
<?php
var_dump(substr('x', PHP_INT_MIN));
var_dump(substr('x', 0, PHP_INT_MIN));
?>
--EXPECT--
string(1) "x"
string(0) ""
