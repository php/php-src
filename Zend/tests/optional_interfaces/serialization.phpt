--TEST--
Classes with optional interfaces survive serialization
--FILE--
<?php

interface ExistingInterface {}
class TestClass implements ?ExistingInterface, ?NonExistantInterface {}

$original = new TestClass;
$deserialized = unserialize(serialize($original));

echo implode(',', class_implements($deserialized));

?>
--EXPECT--
ExistingInterface
