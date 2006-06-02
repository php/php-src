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

Warning: Wrong parameter count for (null)() in %s on line %d
