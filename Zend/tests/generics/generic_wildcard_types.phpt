--TEST--
Generic wildcard/bounded types
--FILE--
<?php

class Animal {}
class Dog extends Animal {}
class Cat extends Animal {}
class GoldenRetriever extends Dog {}

class Box<T> {
    private T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

// === Upper bounded wildcard: ? extends Animal ===
function acceptUpperBounded(Box<? extends Animal> $box): void {
    echo "Upper bounded accepted: " . get_class($box->get()) . "\n";
}

// Dog extends Animal — should pass
$dogBox = new Box<Dog>(new Dog());
acceptUpperBounded($dogBox);

// Cat extends Animal — should pass
$catBox = new Box<Cat>(new Cat());
acceptUpperBounded($catBox);

// Animal itself — should pass (exact match)
$animalBox = new Box<Animal>(new Animal());
acceptUpperBounded($animalBox);

// GoldenRetriever extends Dog extends Animal — should pass
$grBox = new Box<GoldenRetriever>(new GoldenRetriever());
acceptUpperBounded($grBox);

// int is not a subtype of Animal — should fail
$intBox = new Box<int>(42);
try {
    acceptUpperBounded($intBox);
    echo "ERROR: should have failed\n";
} catch (TypeError $e) {
    echo "Upper bounded rejected int: OK\n";
}

// === Lower bounded wildcard: ? super Dog ===
function acceptLowerBounded(Box<? super Dog> $box): void {
    echo "Lower bounded accepted: type OK\n";
}

// Animal is a supertype of Dog — should pass
acceptLowerBounded($animalBox);

// Dog itself — should pass (exact match)
acceptLowerBounded($dogBox);

// Cat is NOT a supertype of Dog — should fail
try {
    acceptLowerBounded($catBox);
    echo "ERROR: should have failed\n";
} catch (TypeError $e) {
    echo "Lower bounded rejected Cat: OK\n";
}

// GoldenRetriever is a subtype of Dog, not a supertype — should fail
try {
    acceptLowerBounded($grBox);
    echo "ERROR: should have failed\n";
} catch (TypeError $e) {
    echo "Lower bounded rejected GoldenRetriever: OK\n";
}

// === Unbounded wildcard: ? ===
function acceptUnbounded(Box<?> $box): void {
    echo "Unbounded accepted\n";
}

// All should pass
acceptUnbounded($dogBox);
acceptUnbounded($catBox);
acceptUnbounded($animalBox);
acceptUnbounded($intBox);

echo "Done.\n";
?>
--EXPECT--
Upper bounded accepted: Dog
Upper bounded accepted: Cat
Upper bounded accepted: Animal
Upper bounded accepted: GoldenRetriever
Upper bounded rejected int: OK
Lower bounded accepted: type OK
Lower bounded accepted: type OK
Lower bounded rejected Cat: OK
Lower bounded rejected GoldenRetriever: OK
Unbounded accepted
Unbounded accepted
Unbounded accepted
Unbounded accepted
Done.
