--TEST--
Bug #74251 Compile-time error for placing optional parameters before required parameters
--FILE--
<?php

function test($a = true, $b){}

?>
--EXPECTF--
Fatal error: Only the last parameters can be optional in %sbug74251.php on line %d
