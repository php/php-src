--TEST--
Generic class: type inference respects weak type coercion rules
--FILE--
<?php
// No strict_types — weak mode coercion applies

class Box<T> {
    private T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
    public function set(T $value): void { $this->value = $value; }
}

// Infer T=int
$box = new Box(42);

// Non-numeric string cannot coerce to int even in weak mode
try {
    $box->set("hello");
} catch (TypeError $e) {
    echo "Caught: non-numeric string to int\n";
}

// Infer T=string
$sbox = new Box("world");

// Int can coerce to string in weak mode — should succeed
$sbox->set(123);
echo "After set(123): " . $sbox->get() . "\n";

echo "OK\n";
?>
--EXPECT--
Caught: non-numeric string to int
After set(123): 123
OK
