--TEST--
Generic classes work with JIT enabled
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=1255
opcache.jit_buffer_size=64M
--FILE--
<?php

class Box<T> {
    private T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

// Basic generic instantiation
$box = new Box<int>(42);
echo $box->get() . "\n";

// Type enforcement
try {
    $box2 = new Box<int>("hello");
} catch (TypeError $e) {
    echo "TypeError: OK\n";
}

// Return type enforcement
class Container<T> {
    private T $item;
    public function __construct(T $item) { $this->item = $item; }
    public function getItem(): T { return $this->item; }
}

$c = new Container<string>("world");
echo $c->getItem() . "\n";

// Multiple calls to trigger JIT compilation
for ($i = 0; $i < 100; $i++) {
    $b = new Box<int>($i);
    $b->get();
}
echo "Hot loop: OK\n";

// Inheritance with bound generic args
class IntBox extends Box<int> {}
$ib = new IntBox(99);
echo $ib->get() . "\n";

echo "Done.\n";
?>
--EXPECTF--
42
TypeError: OK
world
Hot loop: OK
99
Done.
