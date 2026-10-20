--TEST--
Test ReflectionProperty::getMangledName() method
--FILE--
<?php

class TestClass {
    public $publicProp = 'public';
    protected $protectedProp = 'protected';
    private $privateProp = 'private';
}

function testMangledName($class, $property) {
    $reflection = new ReflectionProperty($class, $property);
    echo "Property: $property\n";
    echo "getName(): " . $reflection->getName() . "\n";
    echo "getMangledName(): " . $reflection->getMangledName() . "\n";
    
    $obj = new $class();
    $array = (array) $obj;
    echo "In array cast: " . (array_key_exists($reflection->getMangledName(), $array) ? "found" : "not found") . "\n";
    echo "\n";
}

testMangledName('TestClass', 'publicProp');
testMangledName('TestClass', 'protectedProp');  
testMangledName('TestClass', 'privateProp');

?>
--EXPECTF--
Property: publicProp
getName(): publicProp
getMangledName(): publicProp
In array cast: found

Property: protectedProp
getName(): protectedProp
getMangledName(): %0*%0protectedProp
In array cast: found

Property: privateProp
getName(): privateProp
getMangledName(): %0TestClass%0privateProp
In array cast: found
