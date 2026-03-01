--TEST--
Generic class: type inference from constructor arguments
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    private T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
    public function set(T $value): void { $this->value = $value; }
}

// Infer T=int from constructor arg
$box = new Box(42);
echo $box->get() . "\n";

// Should TypeError: T was inferred as int, string is incompatible
try {
    $box->set("hello");
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

// Infer T=string from constructor arg
$sbox = new Box("world");
echo $sbox->get() . "\n";

// Should TypeError: T was inferred as string, int is incompatible (strict mode)
try {
    $sbox->set(123);
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

echo "OK\n";
?>
--EXPECTF--
42
Box::set(): Argument #1 ($value) must be of type int, string given, called in %s on line %d
world
Box::set(): Argument #1 ($value) must be of type string, int given, called in %s on line %d
OK
