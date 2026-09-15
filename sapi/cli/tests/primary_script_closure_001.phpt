--TEST--
Execute closures returned by the primary script
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

return function () {
	echo "Called", PHP_EOL;
};

?>
--EXPECT--
Called
