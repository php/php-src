--TEST--
Test typed properties allow null
--FILE--
<?php
class Foo {
    public null $value;
}

$foo = new Foo();
?>
===DONE===
--EXPECT--
===DONE===
