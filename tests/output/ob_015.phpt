--TEST--
output buffering - failure
--FILE--
<?php
ob_start("str_rot13", 1);
echo "foo\n";
?>
--EXPECTF--
foo

Warning: Wrong parameter count for str_rot13() in %s on line %d
