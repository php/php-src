--TEST--
Backed readonly property with hooks in abstract class
--FILE--
<?php
abstract class Test {

    public readonly int $prop {
        get => $this->prop;
        set => $value;
    }

    public function __construct(int $v) {
        $this->prop = $v;
    }

    public function set(int $v) {
        $this->prop = $v;
    }
}

class Child extends Test {}

$ch = new Child(42);
var_dump($ch->prop);
try {
    $ch->set(43);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $ch->prop = 43;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($ch->prop);
?>
--EXPECT--
int(42)
Error: Cannot modify readonly property Test::$prop
Error: Cannot modify protected(set) readonly property Test::$prop from global scope
int(42)
