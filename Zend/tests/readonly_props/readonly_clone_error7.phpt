--TEST--
__clone() cannot indirectly modify unlocked readonly properties
--FILE--
<?php

class Foo {
    public function __construct(
        public readonly array $bar
    ) {}

    public function __clone()
    {
        $this->bar['bar'] = 'bar';
    }
}

$foo = new Foo([]);
// First call fills the cache slot
try {
    var_dump(clone $foo);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump(clone $foo);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot indirectly modify readonly property Foo::$bar
Error: Cannot indirectly modify readonly property Foo::$bar
