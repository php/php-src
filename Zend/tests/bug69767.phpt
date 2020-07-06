--TEST--
Bug #69767 (Default parameter value with wrong type segfaults)
--FILE--
<?php
function foo(String $bar = 0) {}
?>
--EXPECTF--
Fatal error: foo(): Parameter #1 ($bar) of type string cannot have a default value of type int in %s on line %d
