--TEST--
Bug GH-20914: Internal enums can be compared
--EXTENSIONS--
zend_test
--FILE--
<?php

$foo = ZendTestUnitEnum::Foo;
$bar = ZendTestUnitEnum::Bar;

var_dump($foo === $foo);
var_dump($foo == $foo);

var_dump($foo === $bar);
var_dump($foo == $bar);

var_dump($bar === $foo);
var_dump($bar == $foo);

var_dump($foo > $foo);
var_dump($foo < $foo);
var_dump($foo >= $foo);
var_dump($foo <= $foo);

var_dump($foo > $bar);
var_dump($foo < $bar);
var_dump($foo >= $bar);
var_dump($foo <= $bar);

var_dump($foo > true);
var_dump($foo < true);
var_dump($foo >= true);
var_dump($foo <= true);

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
