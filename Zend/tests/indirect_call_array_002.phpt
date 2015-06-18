--TEST--
Indirect method call by array - Invalid method name
--FILE--
<?php

$arr = array('stdclass', 'b');
$arr();

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined method stdClass::b() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
