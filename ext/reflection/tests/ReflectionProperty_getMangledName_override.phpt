--TEST--
Test ReflectionProperty::getMangledName() with property overrides and visibility changes
--FILE--
<?php

class Parent1 {
    public $prop = 'parent_public';
    protected $protectedProp = 'parent_protected';
    private $privateProp = 'parent_private';
}

class Child1 extends Parent1 {
    // same visibility
    public $prop = 'child_public';
    
    // overridden visibility
    public $protectedProp = 'child_public_from_protected';
}

class Parent2 {
    protected $visibilityTest = 'parent_protected';
}

class Child2 extends Parent2 {
    // overriden visibility
    public $visibilityTest = 'child_public';
}

function testPropertyOverride($class, $property, $description) {
    echo "$description:\n";
    $reflection = new ReflectionProperty($class, $property);
    echo "  Class: $class\n";
    echo "  Property: $property\n";
    echo "  getName(): " . $reflection->getName() . "\n";
    echo "  getMangledName(): " . $reflection->getMangledName() . "\n";
    echo "  Visibility: " . ($reflection->isPublic() ? 'public' : ($reflection->isProtected() ? 'protected' : 'private')) . "\n";
    
    $obj = new $class();
    $array = (array) $obj;
    echo "  Found in array cast: " . (array_key_exists($reflection->getMangledName(), $array) ? "yes" : "no") . "\n";
    echo "\n";
}

testPropertyOverride('Parent1', 'prop', 'Parent public property');
testPropertyOverride('Child1', 'prop', 'Child public property (overrides parent public)');

testPropertyOverride('Parent1', 'protectedProp', 'Parent protected property');
testPropertyOverride('Child1', 'protectedProp', 'Child public property (overrides parent protected)');

testPropertyOverride('Parent2', 'visibilityTest', 'Parent protected property');
testPropertyOverride('Child2', 'visibilityTest', 'Child public property (overrides parent protected)');

testPropertyOverride('Parent1', 'privateProp', 'Parent private property');

echo "Child1 instance array keys:\n";
$child1 = new Child1();
$array = (array) $child1;
foreach (array_keys($array) as $key) {
    echo "  '$key'\n";
}

?>
--EXPECTF--
Parent public property:
  Class: Parent1
  Property: prop
  getName(): prop
  getMangledName(): prop
  Visibility: public
  Found in array cast: yes

Child public property (overrides parent public):
  Class: Child1
  Property: prop
  getName(): prop
  getMangledName(): prop
  Visibility: public
  Found in array cast: yes

Parent protected property:
  Class: Parent1
  Property: protectedProp
  getName(): protectedProp
  getMangledName(): %0*%0protectedProp
  Visibility: protected
  Found in array cast: yes

Child public property (overrides parent protected):
  Class: Child1
  Property: protectedProp
  getName(): protectedProp
  getMangledName(): protectedProp
  Visibility: public
  Found in array cast: yes

Parent protected property:
  Class: Parent2
  Property: visibilityTest
  getName(): visibilityTest
  getMangledName(): %0*%0visibilityTest
  Visibility: protected
  Found in array cast: yes

Child public property (overrides parent protected):
  Class: Child2
  Property: visibilityTest
  getName(): visibilityTest
  getMangledName(): visibilityTest
  Visibility: public
  Found in array cast: yes

Parent private property:
  Class: Parent1
  Property: privateProp
  getName(): privateProp
  getMangledName(): %0Parent1%0privateProp
  Visibility: private
  Found in array cast: yes

Child1 instance array keys:
  'prop'
  'protectedProp'
  '%0Parent1%0privateProp'
