--TEST--
Test typed properties delay type check on constant
--FILE--
<?php
class Foo {
    public int $bar = BAR::BAZ;
}

try {
    $foo = new Foo();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Class "BAR" not found
