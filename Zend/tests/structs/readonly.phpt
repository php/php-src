--TEST--
Structs may be marked as readonly
--FILE--
<?php

readonly struct Box {
    public function __construct(
        public mixed $value,
    ) {}
}

$a = new Box(42);
$b = new Box(42);
var_dump($a === $b);

try {
    $b->value = 43;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
bool(true)
Cannot modify readonly property Box::$value
