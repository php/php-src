--TEST--
basic union disallow void param
--FILE--
<?php
function(Foo | void $throw) {};
?>
--EXPECTF--
Fatal error: void cannot be used as a parameter type in %s on line 2
