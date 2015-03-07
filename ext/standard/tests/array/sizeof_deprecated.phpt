--TEST--
Test sizeof() function : deprecated
--FILE--
<?php

var_dump(sizeof([1, 2, 3]));
?>
--EXPECTF--
Deprecated: Function sizeof() is deprecated in %s on line %d
int(3)