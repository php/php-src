--TEST--
Indirect method call by array - Invalid class name
--FILE--
<?php

$arr = array('a', 'b');
$arr();

?>
--EXPECTF--
Fatal error: Uncaught Error: Class 'a' not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
