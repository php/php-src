--TEST--
Test ReflectionProperty::getMangledName() from instance vs class
--FILE--
<?php

#[AllowDynamicProperties]
class TestClass {
    public $public = 'public';
    protected $protected = 'protected';
    private $private = 'private';
}

$obj = new TestClass();
$obj->dynamic = 'dynamic';

echo "=== Testing ReflectionProperty from CLASS ===\n";

function testFromClass($property) {
    try {
        $reflection = new ReflectionProperty('TestClass', $property);
        echo "Property $property from class:\n";
        echo "  getName(): " . $reflection->getName() . "\n";
        echo "  getMangledName(): " . $reflection->getMangledName() . "\n";
        echo "\n";
    } catch (ReflectionException $e) {
        echo "Property $property from class: EXCEPTION - " . $e->getMessage() . "\n\n";
    }
}

testFromClass('public');
testFromClass('protected');
testFromClass('private');
testFromClass('dynamic');

echo "=== Testing ReflectionProperty from INSTANCE ===\n";

function testFromInstance($obj, $property) {
    try {
        $reflection = new ReflectionProperty($obj, $property);
        echo "Property $property from instance:\n";
        echo "  getName(): " . $reflection->getName() . "\n";
        echo "  getMangledName(): " . $reflection->getMangledName() . "\n";
        
        $array = (array) $obj;
        echo "  Found in array cast: " . (array_key_exists($reflection->getMangledName(), $array) ? "yes" : "no") . "\n";
        echo "\n";
    } catch (ReflectionException $e) {
        echo "Property $property from instance: EXCEPTION - " . $e->getMessage() . "\n\n";
    }
}

testFromInstance($obj, 'public');
testFromInstance($obj, 'protected');
testFromInstance($obj, 'private');

echo "=== Instance array keys ===\n";
$array = (array) $obj;
foreach (array_keys($array) as $key) {
    echo "Key: '$key'\n";
}

?>
--EXPECTF--
=== Testing ReflectionProperty from CLASS ===
Property public from class:
  getName(): public
  getMangledName(): public

Property protected from class:
  getName(): protected
  getMangledName(): %0*%0protected

Property private from class:
  getName(): private
  getMangledName(): %0TestClass%0private

Property dynamic from class: EXCEPTION - Property TestClass::$dynamic does not exist

=== Testing ReflectionProperty from INSTANCE ===
Property public from instance:
  getName(): public
  getMangledName(): public
  Found in array cast: yes

Property protected from instance:
  getName(): protected
  getMangledName(): %0*%0protected
  Found in array cast: yes

Property private from instance:
  getName(): private
  getMangledName(): %0TestClass%0private
  Found in array cast: yes

=== Instance array keys ===
Key: 'public'
Key: '%0*%0protected'
Key: '%0TestClass%0private'
Key: 'dynamic'
