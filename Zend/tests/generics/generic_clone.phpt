--TEST--
Generic class: clone preserves generic type arguments
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
}

$a = new Box<int>(42);
$b = clone $a;

echo $b->value . "\n";

// Clone should preserve generic args — assigning wrong type should fail
try {
    $b->value = "hello";
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

echo "OK\n";
?>
--EXPECTF--
42
Cannot assign string to property Box::$value of type int
OK
