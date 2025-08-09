--TEST--
Destructuring with list() a value of type object (that does not implement ArrayAccess)
--FILE--
<?php

$v = new stdClass();

list($a, $b) = $v;

var_dump($a, $b);

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use object of type stdClass as array in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
