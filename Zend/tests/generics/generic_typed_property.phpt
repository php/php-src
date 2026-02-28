--TEST--
Generic class: typed property with generic param
--FILE--
<?php
class Box<T> {
    private T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
    public function set(T $value): void { $this->value = $value; }
}

$box = new Box<int>(42);
echo $box->get() . "\n";

$box->set(99);
echo $box->get() . "\n";

// Should TypeError: method argument type enforcement catches the wrong type
try {
    $box->set("hello");
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
42
99
Box::set(): Argument #1 ($value) must be of type int, string given, called in %s on line %d
