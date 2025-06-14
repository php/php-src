--TEST--
Backed property in readonly class may have hooks
--FILE--
<?php

// readonly class
readonly class Test {
    public int $prop {
        get => $this->prop;
        set => $value;
    }

    public function __construct(int $v) {
        $this->prop = $v;
    }

    public function set($v)
    {
         $this->prop = $v;
    }
}

$t = new Test(42);
var_dump($t->prop);
try {
    $t->set(43);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $t->prop = 43;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($t->prop);
?>
--EXPECT--
int(42)
Error: Cannot modify readonly property Test::$prop
Error: Cannot modify protected(set) readonly property Test::$prop from global scope
int(42)
