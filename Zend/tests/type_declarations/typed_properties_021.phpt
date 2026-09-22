--TEST--
Test typed properties delay type check on constant
--FILE--
<?php
class Foo {
    public int $bar = BAR::BAZ;
}

try {
    $foo = new Foo();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Class "BAR" not found
