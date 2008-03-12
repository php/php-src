--TEST--
Trying use object of type stdClass as array
--FILE--
<?php

$a[0] = new stdclass;
$a[0][0] = new stdclass;

?>
--EXPECTF--
Fatal error: Cannot use object of type stdClass as array in %s on line %d
