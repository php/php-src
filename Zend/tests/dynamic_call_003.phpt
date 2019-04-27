--TEST--
Testing dynamic call with invalid method name
--FILE--
<?php

$a = new stdClass;
$b = 1;

$a::$b();

?>
--EXPECTF--
Fatal error: Uncaught Error: Function name must be a string in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
