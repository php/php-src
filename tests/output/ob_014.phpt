--TEST--
output buffering - failure
--FILE--
<?php
/*
 * apparently the error handler cannot get the current function name on shutdown
 */
ob_start("str_rot13");
echo "foo\n";
?>
--EXPECTF--
foo

Warning: (null)() expects exactly 1 parameter, 2 given in %s on line %d
