--TEST--
Generic class: debug display with var_dump, print_r, and stack traces
--FILE--
<?php

class Box<T> {
    public function __construct(public T $value) {}
    public function throwError(): void {
        throw new RuntimeException("test");
    }
}

class Pair<A, B> {
    public function __construct(public A $first, public B $second) {}
}

// var_dump shows generic args
$b = new Box<int>(42);
var_dump($b);

// print_r shows generic args
echo "---\n";
$p = new Pair<string, int>("hello", 99);
print_r($p);

// get_class returns raw class name (no generic args)
echo "---\n";
echo get_class($b) . "\n";
echo get_class($p) . "\n";

// Stack trace shows generic args
echo "---\n";
try {
    $b->throwError();
} catch (Throwable $e) {
    echo $e->getTraceAsString() . "\n";
}

// Inherited class with bound generic args
class IntBox extends Box<int> {}
$ib = new IntBox(100);
var_dump($ib);

echo "OK\n";
?>
--EXPECTF--
object(Box<int>)#1 (1) {
  ["value"]=>
  int(42)
}
---
Pair<string, int> Object
(
    [first] => hello
    [second] => 99
)
---
Box
Pair
---
#0 %s(%d): Box<int>->throwError()
#1 {main}
object(IntBox<int>)#%d (1) {
  ["value"]=>
  int(100)
}
OK
