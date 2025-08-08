--TEST--
Testing array with '[]' passed as argument by value
--FILE--
<?php

function test($var) { }
test($arr[]);

?>
--EXPECTF--
Fatal error: Cannot use [] for reading in %s on line %d
