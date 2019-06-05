--TEST--
Bug #72162 (use-after-free - error_reporting)
--FILE--
<?php
error_reporting(E_ALL);
$var11 = new StdClass();
$var16 = error_reporting($var11);
?>
--EXPECTF--
Fatal error: Uncaught Error: Object of class stdClass could not be converted to string in %s:%d
Stack trace:
#0 %s(%d): error_reporting(Object(stdClass))
#1 {main}
  thrown in %s on line %d
