--TEST--
ReflectionClass::getName - forbid static invocation
--FILE--
<?php
ReflectionClass::getName();
?>
--EXPECTF--
Fatal error: Uncaught Error: Non-static method ReflectionClass::getName() cannot be called statically in %s:2
Stack trace:
#0 {main}
  thrown in %s on line 2
