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

$ref = new ReflectionClass(Foo::class);
$obj = $ref->newInstanceWithoutConstructor();

// Property is uninitialized at this point
try {
    var_dump($obj->bar);
} catch (Throwable $e) {
    echo get_class($e), ": ", $e->getMessage(), "\n";
}

// First constructor call: CPP reassignment allowed
$obj->__construct('explicit call');
var_dump($obj->bar);

// Second call fails: property no longer uninitialized
try {
    $obj->__construct('second call');
} catch (Throwable $e) {
    echo get_class($e), ": ", $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Typed property Foo::$bar must not be accessed before initialization
string(26) "overwritten in constructor"
Error: Cannot modify readonly property Foo::$bar
