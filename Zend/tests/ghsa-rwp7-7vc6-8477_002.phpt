--TEST--
GHSA-rwp7-7vc6-8477: Use-after-free for ??= due to incorrect live-range calculation
--FILE--
<?php

class Foo {
    public int $prop;

    public function foo() {
        return $this;
    }
}

$foo = new Foo();

try {
    $foo->foo()->prop ??= 'foo';
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Cannot assign string to property Foo::$prop of type int
