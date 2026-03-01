--TEST--
Generic class: basic declaration and instantiation
--FILE--
<?php
class Box<T> {
    private $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

$box = new Box(42);
echo $box->get() . "\n";

$box2 = new Box("hello");
echo $box2->get() . "\n";
?>
--EXPECT--
42
hello
