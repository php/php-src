--TEST--
Bug #69767 (Default parameter value with wrong type segfaults)
--FILE--
<?php
function foo(String $bar = 0) {}
?>
--EXPECTF--
Fatal error: Cannot use int as default value for parameter $bar of type string in %s on line %d
