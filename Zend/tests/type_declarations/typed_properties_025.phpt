--TEST--
Test typed properties type must preceed first declaration in group
--FILE--
<?php
class Foo {
    public $bar,
           int $qux;
}
?>
--EXPECTF--
Parse error: syntax error, unexpected 'int' (T_STRING), expecting variable (T_VARIABLE) in %s on line 4
