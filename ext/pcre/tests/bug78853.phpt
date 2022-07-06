--TEST--
Bug #78853 (preg_match() may return integer > 1)
--FILE--
<?php
var_dump(preg_match('/^|\d{1,2}$/', "7"));
?>
--EXPECT--
int(1)
