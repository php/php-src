--TEST--
Test typed properties allow false
--FILE--
<?php
class Foo {
    public false $value;
}

$foo = new Foo();
?>
===DONE===
--EXPECT--
===DONE===
