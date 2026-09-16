--TEST--
Test ReflectionProperty::getMangledName() with inheritance
--FILE--
<?php

class ParentClass {
    public $public = 'parent_public';
    protected $protected = 'parent_protected';
    private $private = 'parent_private';
    private $parentOnly = 'parent_only_private';
}

class ChildClass extends ParentClass {
    private $private = 'child_private';
    protected $childProp = 'child_protected';
}

function testProperty($class, $property) {
    $reflection = new ReflectionProperty($class, $property);
    $obj = new $class();
    $array = (array) $obj;
    
    echo "Class: $class, Property: \$$property\n";
    echo "Mangled name: '" . $reflection->getMangledName() . "'\n";
    echo "Key exists in array cast: " . (array_key_exists($reflection->getMangledName(), $array) ? "yes" : "no") . "\n";
    echo "\n";
}

testProperty('ParentClass', 'public');
testProperty('ParentClass', 'protected');
testProperty('ParentClass', 'private');

testProperty('ChildClass', 'public');
testProperty('ChildClass', 'protected');
testProperty('ChildClass', 'childProp');
testProperty('ChildClass', 'private');

echo "Access to parent's private property not in child:\n";

try {
    $reflection = new ReflectionProperty('ChildClass', 'parentOnly');
    echo "ERROR: Should have failed\n";
} catch (ReflectionException $e) {
    echo "Instance-based creation failed as expected: " . $e->getMessage() . "\n";
}

try {
    $obj = new ChildClass();
    $reflection = new ReflectionProperty($obj, 'parentOnly');
    echo "ERROR: Should have failed\n";
} catch (ReflectionException $e) {
    echo "Object-based creation failed as expected: " . $e->getMessage() . "\n";
}

?>
--EXPECTF--
Class: ParentClass, Property: $public
Mangled name: 'public'
Key exists in array cast: yes

Class: ParentClass, Property: $protected
Mangled name: '%0*%0protected'
Key exists in array cast: yes

Class: ParentClass, Property: $private
Mangled name: '%0ParentClass%0private'
Key exists in array cast: yes

Class: ChildClass, Property: $public
Mangled name: 'public'
Key exists in array cast: yes

Class: ChildClass, Property: $protected
Mangled name: '%0*%0protected'
Key exists in array cast: yes

Class: ChildClass, Property: $childProp
Mangled name: '%0*%0childProp'
Key exists in array cast: yes

Class: ChildClass, Property: $private
Mangled name: '%0ChildClass%0private'
Key exists in array cast: yes

Access to parent's private property not in child:
Instance-based creation failed as expected: Property ChildClass::$parentOnly does not exist
Object-based creation failed as expected: Property ChildClass::$parentOnly does not exist
