--TEST--
ReflectionClass::getName - forbid static invocation
--FILE--
<?php
ReflectionClass::getName();
?>
--EXPECTF--
Fatal error: Non-static method ReflectionClass::getName() cannot be called statically in %s on line 2
