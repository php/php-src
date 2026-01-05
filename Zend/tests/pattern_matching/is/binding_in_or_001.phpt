--TEST--
Must not bind to variables in | pattern
--FILE--
<?php
var_dump(42 is $foo|$bar);
?>
--EXPECTF--
Fatal error: Must not bind to variables inside | pattern in %s on line %d
