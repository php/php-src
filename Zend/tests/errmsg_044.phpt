--TEST--
Trying use object of type StdClass as array
--FILE--
<?php

$a[0] = new StdClass;
$a[0][0] = new StdClass;

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use object of type StdClass as array in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
