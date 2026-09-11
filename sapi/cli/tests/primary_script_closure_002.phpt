--TEST--
Execute closures returned by the primary script (First Class Callable)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

function main() {
	echo "Called", PHP_EOL;
}

return main(...);

?>
--EXPECT--
Called
