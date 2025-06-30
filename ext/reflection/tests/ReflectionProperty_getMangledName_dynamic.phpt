--TEST--
Test ReflectionProperty::getMangledName() with dynamic properties
--FILE--
<?php

echo "=== Testing stdClass with dynamic properties ===\n";
$stdObj = new stdClass();
$stdObj->prop1 = 'value1';
$stdObj->{'special-name'} = 'special value';
$stdObj->{'123numeric'} = 'numeric start';

function testDynamicProperty($obj, $property, $description) {
    try {
        $reflection = new ReflectionProperty($obj, $property);
        echo "$description:\n";
        echo "  getName(): " . $reflection->getName() . "\n";
        echo "  getMangledName(): " . $reflection->getMangledName() . "\n";
        
        $array = (array) $obj;
        echo "  Found in array cast: " . (array_key_exists($reflection->getMangledName(), $array) ? "yes" : "no") . "\n";
        echo "\n";
    } catch (ReflectionException $e) {
        echo "$description: EXCEPTION - " . $e->getMessage() . "\n\n";
    }
}

testDynamicProperty($stdObj, 'prop1', 'stdClass property prop1');
testDynamicProperty($stdObj, 'special-name', 'stdClass property with special name');
testDynamicProperty($stdObj, '123numeric', 'stdClass property starting with number');

echo "=== Testing edge cases ===\n";
$numericObj = (object)[true];
testDynamicProperty($numericObj, '0', 'Property name as number');

$nullByteObj = (object)["foo\0" => true];
testDynamicProperty($nullByteObj, "foo\0", 'Property name with null byte');

$invalidObj = (object)["::" => true];
testDynamicProperty($invalidObj, '::', 'Invalid property name');

echo "=== Testing regular class with dynamic properties ===\n";
#[AllowDynamicProperties]
class TestClass {
    public $existing = 'existing';
}

$obj = new TestClass();
$obj->dynamic = 'dynamic value';
$obj->anotherDynamic = 'another dynamic';

testDynamicProperty($obj, 'dynamic', 'Regular class dynamic property');
testDynamicProperty($obj, 'anotherDynamic', 'Regular class another dynamic property');

$reflection = new ReflectionProperty($obj, 'existing');
echo "Regular property:\n";
echo "  getName(): " . $reflection->getName() . "\n";
echo "  getMangledName(): " . $reflection->getMangledName() . "\n";

echo "\n=== Testing ReflectionProperty from class vs instance ===\n";
try {
    $reflection = new ReflectionProperty('TestClass', 'dynamic');
    echo "This should not be reached\n";
} catch (ReflectionException $e) {
    echo "Expected exception for class-based reflection: " . $e->getMessage() . "\n";
}

try {
    $reflection = new ReflectionProperty($obj, 'dynamic');
    echo "Instance-based reflection works: " . $reflection->getMangledName() . "\n";
} catch (ReflectionException $e) {
    echo "Unexpected exception: " . $e->getMessage() . "\n";
}

?>
--EXPECTF--
=== Testing stdClass with dynamic properties ===
stdClass property prop1:
  getName(): prop1
  getMangledName(): prop1
  Found in array cast: yes

stdClass property with special name:
  getName(): special-name
  getMangledName(): special-name
  Found in array cast: yes

stdClass property starting with number:
  getName(): 123numeric
  getMangledName(): 123numeric
  Found in array cast: yes

=== Testing edge cases ===
Property name as number:
  getName(): 0
  getMangledName(): 0
  Found in array cast: yes

Property name with null byte:
  getName(): foo%0
  getMangledName(): foo%0
  Found in array cast: yes

Invalid property name:
  getName(): ::
  getMangledName(): ::
  Found in array cast: yes

=== Testing regular class with dynamic properties ===
Regular class dynamic property:
  getName(): dynamic
  getMangledName(): dynamic
  Found in array cast: yes

Regular class another dynamic property:
  getName(): anotherDynamic
  getMangledName(): anotherDynamic
  Found in array cast: yes

Regular property:
  getName(): existing
  getMangledName(): existing

=== Testing ReflectionProperty from class vs instance ===
Expected exception for class-based reflection: Property TestClass::$dynamic does not exist
Instance-based reflection works: dynamic
