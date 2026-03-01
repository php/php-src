--TEST--
Generic class: type enforcement on constructor parameter
--FILE--
<?php
class Box<T> {
    private $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

try {
    $box = new Box<int>("not an int");
} catch (TypeError $e) {
    echo "TypeError: " . $e->getMessage() . "\n";
}
?>
--EXPECTF--
TypeError: Box::__construct(): Argument #1 ($value) must be of type int, string given, called in %s on line %d
