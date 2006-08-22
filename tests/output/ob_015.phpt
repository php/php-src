--TEST--
output buffering - failure
--FILE--
<?php
ob_start("str_rot13", 1);
echo "foo\n";
?>
--EXPECTF--
foo

Warning: str_rot13() expects exactly 1 parameter, 2 given in %s on line %d
