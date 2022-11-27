--TEST--
Must not bind to variables in | pattern
--FILE--
<?php
var_dump(42 is 42|(43 & $bar));
?>
--EXPECTF--
Fatal error: Must not bind to variables inside | pattern in %s on line %d
