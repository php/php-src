--TEST--
Generic class: nested generics with >> parsing and type enforcement
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

// Nested generics: Box<Box<int>> — tests >> parsing
$inner = new Box<int>(42);
$outer = new Box<Box<int>>($inner);

echo $outer->get()->get() . "\n";

// Wrong type on outer (expects Box<int>, got string)
try {
    $outer->value = "hello";
} catch (TypeError $e) {
    echo "outer: caught\n";
}

// Wrong type on inner (expects int, got string)
try {
    $outer->get()->value = "hello";
} catch (TypeError $e) {
    echo "inner: caught\n";
}

echo "OK\n";
?>
--EXPECT--
42
outer: caught
inner: caught
OK
