--TEST--
Promoted readonly properties cannot be reassigned when __construct() is called directly
--FILE--
<?php

class Foo {
    public function __construct(
        public readonly string $value = 'default',
    ) {
        // Don't use reassignment here, leave it available
    }
}

$obj = new Foo('initial');
echo "Initial value: " . $obj->value . "\n";

// Direct call to __construct() should NOT allow reassignment
try {
    $obj->__construct('modified');
    echo "After direct __construct: " . $obj->value . "\n";
} catch (Error $e) {
    echo "Error: " . $e->getMessage() . "\n";
}

// Also test with a class that uses reassignment
class Bar {
    public function __construct(
        public readonly string $value = 'default',
    ) {
        $this->value = strtoupper($this->value);
    }
}

$bar = new Bar('hello');
echo "Bar initial value: " . $bar->value . "\n";

// Direct call should fail during the CPP assignment (property not UNINIT)
// Note: The error happens inside the constructor because CPP assignment happens first
try {
    $bar->__construct('world');
    echo "After direct __construct: " . $bar->value . "\n";
} catch (Error $e) {
    echo "Error: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
Initial value: initial
Error: Cannot modify readonly property Foo::$value
Bar initial value: HELLO
Error: Cannot modify readonly property Bar::$value
