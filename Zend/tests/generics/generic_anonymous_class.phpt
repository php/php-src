--TEST--
Generic class: anonymous classes extending generic classes
--FILE--
<?php

class Box<T> {
    public function __construct(public T $value) {}
    public function get(): T { return $this->value; }
}

// Anonymous class extending generic with bound args
$obj = new class(42) extends Box<int> {};
echo $obj->get() . "\n";

// Type enforcement works
try {
    $obj->value = "not an int";
} catch (TypeError $e) {
    echo "TypeError: type enforced\n";
}

// Anonymous class can override methods
$obj2 = new class("hello") extends Box<string> {
    public function get(): string {
        return strtoupper(parent::get());
    }
};
echo $obj2->get() . "\n";

// Anonymous class implementing generic interface
interface Getter<T> {
    public function get(): T;
}

$obj3 = new class implements Getter<int> {
    public function get(): int { return 99; }
};
echo $obj3->get() . "\n";

echo "OK\n";
?>
--EXPECTF--
42
TypeError: type enforced
HELLO
99
OK
