--TEST--
Bug #69767 (Default parameter value with wrong type segfaults)
--FILE--
<?php
function foo(String $bar = 0) {}
?>
--EXPECTF--
Fatal error: Default value for parameters with a string type can only be string or NULL in %sbug69767.php on line %d
