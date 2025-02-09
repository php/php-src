--TEST--
Reflection works on optional interfaces
--FILE--
<?php

interface ExistingInterface {}

class TestClass implements ?ExistingInterface, ?NonexistantInterface {}

$reflection = new ReflectionClass('TestClass');
echo implode(', ', $reflection->getInterfaceNames());

?>
--EXPECT--
ExistingInterface
