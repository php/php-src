--TEST--
Using a variable other than $this as type
--FILE--
<?php

function(): $var {}

?>
--EXPECTF--
Fatal error: Cannot use $var as type in %s on line %d
