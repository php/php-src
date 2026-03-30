--TEST--
ReflectionAttribute::getCurrent() when in the global scope
--FILE--
<?php

echo ReflectionAttribute::getCurrent();

?>
--EXPECTF--
Fatal error: Uncaught Error: Current function was not invoked via ReflectionAttribute::newInstance() in %s:%d
Stack trace:
#0 %s(%d): ReflectionAttribute::getCurrent()
#1 {main}
  thrown in %s on line %d
