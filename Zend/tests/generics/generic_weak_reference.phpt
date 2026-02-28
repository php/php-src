--TEST--
Generic class: weak references to generic objects
--FILE--
<?php

class Box<T> {
    public function __construct(public T $value) {}
}

$b = new Box<int>(42);
$weak = WeakReference::create($b);

// Weak ref should return the generic object
$ref = $weak->get();
echo "Before unset: ";
var_dump($ref);

// Unset the strong reference
unset($b);
unset($ref);

// Weak ref should be null (object garbage collected)
echo "After unset: ";
var_dump($weak->get());

// WeakMap with generic objects
$map = new WeakMap();
$b1 = new Box<string>("hello");
$b2 = new Box<int>(99);

$map[$b1] = "first";
$map[$b2] = "second";

echo "Map count: " . count($map) . "\n";
echo "Map[b1]: " . $map[$b1] . "\n";
echo "Map[b2]: " . $map[$b2] . "\n";

unset($b1);
echo "After unset b1: " . count($map) . "\n";

echo "OK\n";
?>
--EXPECTF--
Before unset: object(Box<int>)#1 (1) {
  ["value"]=>
  int(42)
}
After unset: NULL
Map count: 2
Map[b1]: first
Map[b2]: second
After unset b1: 1
OK
