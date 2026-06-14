--TEST--
Test ReflectionProperty::getMangledName() with property hooks
--FILE--
<?php

echo "=== Testing virtual hooked properties ===\n";

class Demo {
    protected string $foo {
        get => "virtual";
    }
    
    public string $bar {
        get => "hooked getter";
        set => throw new Exception("Cannot set bar");
    }
    
    public string $baz = "backed";
}

$d = new Demo();

function testHookedProperty($obj, $property, $description) {
    try {
        $reflection = new ReflectionProperty($obj, $property);
        echo "$description:\n";
        echo "  getName(): " . $reflection->getName() . "\n";
        echo "  getMangledName(): " . $reflection->getMangledName() . "\n";
        
        $array = (array) $obj;
        echo "  Found in array cast: " . (array_key_exists($reflection->getMangledName(), $array) ? "yes" : "no") . "\n";
        echo "  Has hooks: " . ($reflection->hasHooks() ? "yes" : "no") . "\n";
        echo "\n";
    } catch (ReflectionException $e) {
        echo "$description: EXCEPTION - " . $e->getMessage() . "\n\n";
    }
}

testHookedProperty($d, 'foo', 'Virtual hooked property (protected)');
testHookedProperty($d, 'bar', 'Hooked property with getter/setter (public)');
testHookedProperty($d, 'baz', 'Regular backed property');

echo "=== Object dump ===\n";
var_dump($d);

echo "\n=== Array cast ===\n";
var_dump((array)$d);

?>
--EXPECTF--
=== Testing virtual hooked properties ===
Virtual hooked property (protected):
  getName(): foo
  getMangledName(): %0*%0foo
  Found in array cast: no
  Has hooks: yes

Hooked property with getter/setter (public):
  getName(): bar
  getMangledName(): bar
  Found in array cast: no
  Has hooks: yes

Regular backed property:
  getName(): baz
  getMangledName(): baz
  Found in array cast: yes
  Has hooks: no

=== Object dump ===
object(Demo)#1 (1) {
  ["bar"]=>
  uninitialized(string)
  ["baz"]=>
  string(6) "backed"
}

=== Array cast ===
array(1) {
  ["baz"]=>
  string(6) "backed"
}
