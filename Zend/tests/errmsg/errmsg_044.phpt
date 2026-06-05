--TEST--
Trying use object of type stdClass as array
--FILE--
<?php

$a[0] = new stdClass;
$a[0][0] = new stdClass;

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use object of type stdClass as array in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
