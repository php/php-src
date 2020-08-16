--TEST--
Bug #79969: ReflectionMethod::__construct throws incorrect exception when 2nd parameter is int
--FILE--
<?php
declare(strict_types=1);

new ReflectionMethod('xxx', 1);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: ReflectionMethod::__construct() expects parameter 2 to be string, int given in %s
Stack trace:
#0 %s(%d): ReflectionMethod->__construct('xxx', 1)
#1 {main}
  thrown in %s on line %d
