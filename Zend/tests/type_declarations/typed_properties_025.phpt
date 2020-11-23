--TEST--
Test typed properties type must precede first declaration in group
--FILE--
<?php
class Foo {
    public $bar,
           int $qux;
}
?>
--EXPECTF--
Parse error: syntax error, unexpected identifier "int", expecting variable in %s on line %d
