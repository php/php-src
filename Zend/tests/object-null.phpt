--TEST--
Test whether an object is NULL or not.
--FILE--
<?php

class Bla
{
}

$b = new Bla;

var_dump($b != null);
var_dump($b == null);
var_dump($b !== null);
var_dump($b === null);
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
