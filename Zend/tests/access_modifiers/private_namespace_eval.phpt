--TEST--
private(namespace) visibility in eval'd code
--FILE--
<?php

namespace Foo;

class Test {
    private(namespace) int $prop = 42;

    private(namespace) function method(): string {
        return "called";
    }
}

$obj = new Test();

// Test 1: eval with explicit namespace declaration should work
echo "Test 1: eval with namespace declaration\n";
eval('
namespace Foo;
$val = $obj->prop;
echo "Property access: $val\n";
$result = $obj->method();
echo "Method call: $result\n";
');

// Test 2: eval without namespace declaration should fail
echo "\nTest 2: eval without namespace declaration\n";
try {
    eval('$val = $obj->prop;');
    echo "UNEXPECTED: Should have failed\n";
} catch (\Error $e) {
    echo "Expected error: " . $e->getMessage() . "\n";
}

// Test 3: function defined in eval with namespace should work
echo "\nTest 3: function defined in eval\n";
eval('
namespace Foo;
function testFunc($obj) {
    return $obj->prop * 2;
}
');

$result = \Foo\testFunc($obj);
echo "Function result: $result\n";

// Test 4: eval from within the namespace (top-level code)
echo "\nTest 4: eval accessing same object again\n";
eval('
namespace Foo;
$val = $obj->prop + 10;
echo "Modified access: $val\n";
');

echo "\nDone\n";

?>
--EXPECT--
Test 1: eval with namespace declaration
Property access: 42
Method call: called

Test 2: eval without namespace declaration
Expected error: Cannot access private(namespace) property Foo\Test::$prop from scope {main}

Test 3: function defined in eval
Function result: 84

Test 4: eval accessing same object again
Modified access: 52

Done
