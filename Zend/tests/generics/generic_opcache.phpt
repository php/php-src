--TEST--
Generic classes work with opcache enabled
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

class Box<T> {
    private T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

class Pair<K, V = string> {
    public function __construct(private K $key, private V $val) {}
    public function getKey(): K { return $this->key; }
    public function getVal(): V { return $this->val; }
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

// Default type parameters
$pair = new Pair<int>(42, "age");
echo $pair->getKey() . "\n";
echo $pair->getVal() . "\n";

// Inheritance with bound generic args
class IntBox extends Box<int> {}
$ib = new IntBox(99);
echo $ib->get() . "\n";

// Wildcard types
function acceptAny(Box<?> $b): void {
    echo "Any: OK\n";
}
acceptAny($box);
acceptAny(new Box<string>("hi"));

// var_dump display
var_dump($box);

echo "Done.\n";
?>
--EXPECTF--
42
TypeError: OK
42
age
99
Any: OK
Any: OK
object(Box<int>)#%d (1) {
  ["value":"Box":private]=>
  int(42)
}
Done.
