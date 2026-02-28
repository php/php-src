--TEST--
Generic class: direct typed property assignment enforcement
--FILE--
<?php
class Container<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
}

$c = new Container<int>(42);
echo $c->value . "\n";

// Direct property assignment with correct type
$c->value = 99;
echo $c->value . "\n";

// Direct property assignment with wrong type — should TypeError
try {
    $c->value = "hello";
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
42
99
Cannot assign string to property Container::$value of type int
