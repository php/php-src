--TEST--
Promoted readonly property reassignment works when object created via reflection
--FILE--
<?php

class Foo {
    public function __construct(
        public readonly string $bar = 'default',
    ) {
        $this->bar = 'overwritten in constructor';
    }
}

// Test 1: Object created via reflection without constructor, then __construct() called
echo "Test 1: Reflection newInstanceWithoutConstructor + explicit __construct()\n";
$ref = new ReflectionClass(Foo::class);
$obj = $ref->newInstanceWithoutConstructor();

// Property should be uninitialized at this point
try {
    echo $obj->bar;
    echo "ERROR: Should have thrown for uninitialized property\n";
} catch (Error $e) {
    echo "OK: " . $e->getMessage() . "\n";
}

// Now call constructor - reassignment should work
$obj->__construct('explicit call');
echo "After __construct: " . $obj->bar . "\n";

// Second __construct() call should fail
echo "\nTest 2: Second __construct() call should fail\n";
try {
    $obj->__construct('second call');
    echo "ERROR: Second __construct() should have failed\n";
} catch (Error $e) {
    echo "OK: " . $e->getMessage() . "\n";
}

// Test 3: Normal new still works
echo "\nTest 3: Normal 'new' still works\n";
$obj2 = new Foo('via new');
echo "After new: " . $obj2->bar . "\n";

// Second call should fail
try {
    $obj2->__construct('second call');
    echo "ERROR: Should have failed\n";
} catch (Error $e) {
    echo "OK: " . $e->getMessage() . "\n";
}

// Test 4: Reflection newInstanceArgs (calls constructor)
echo "\nTest 4: Reflection newInstanceArgs\n";
$obj3 = $ref->newInstanceArgs(['via newInstanceArgs']);
echo "After newInstanceArgs: " . $obj3->bar . "\n";

// Second call should fail
try {
    $obj3->__construct('second call');
    echo "ERROR: Should have failed\n";
} catch (Error $e) {
    echo "OK: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
Test 1: Reflection newInstanceWithoutConstructor + explicit __construct()
OK: Typed property Foo::$bar must not be accessed before initialization
After __construct: overwritten in constructor

Test 2: Second __construct() call should fail
OK: Cannot modify readonly property Foo::$bar

Test 3: Normal 'new' still works
After new: overwritten in constructor
OK: Cannot modify readonly property Foo::$bar

Test 4: Reflection newInstanceArgs
After newInstanceArgs: overwritten in constructor
OK: Cannot modify readonly property Foo::$bar
