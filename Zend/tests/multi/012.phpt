--TEST--
basic union disallow void
--FILE--
<?php
function(Foo | void $throw) {};
?>
--EXPECTF--
Fatal error: Void is not a valid parameter type in %s on line 2


