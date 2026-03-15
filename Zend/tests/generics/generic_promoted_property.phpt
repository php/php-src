--TEST--
Generic class: promoted properties with generic type parameters
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    public function __construct(public T $value) {}
}

$intBox = new Box<int>(42);
echo $intBox->value . "\n"; // 42

// Type enforcement on promoted property
try {
    $intBox->value = "hello";
} catch (TypeError $e) {
    echo "Property error: " . $e->getMessage() . "\n";
}

// Constructor type enforcement
try {
    $bad = new Box<int>("hello");
} catch (TypeError $e) {
    echo "Constructor error: " . $e->getMessage() . "\n";
}

// Multiple promoted properties
class Pair<A, B> {
    public function __construct(
        public A $first,
        public B $second,
    ) {}
}

$p = new Pair<string, int>("hello", 42);
echo $p->first . " " . $p->second . "\n";

try {
    $p->first = 123;
} catch (TypeError $e) {
    echo "Pair error: " . $e->getMessage() . "\n";
}

echo "OK\n";
?>
--EXPECTF--
42
Property error: Cannot assign string to property Box::$value of type int
Constructor error: Box::__construct(): Argument #1 ($value) must be of type int, string given, called in %s on line %d
hello 42
Pair error: Cannot assign int to property Pair::$first of type string
OK
