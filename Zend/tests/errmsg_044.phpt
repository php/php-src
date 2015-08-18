--TEST--
Trying use object of type stdClass as array
--FILE--
<?php

$a[0] = new stdclass;
$a[0][0] = new stdclass;

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use object of type stdClass as array in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
