--TEST--
Enum comparison
--FILE--
<?php

enum Foo {
    case Bar;
    case Baz;
}

$bar = Foo::Bar;
$baz = Foo::Baz;

var_dump($bar === $bar);
var_dump($bar == $bar);

var_dump($bar === $baz);
var_dump($bar == $baz);

var_dump($baz === $bar);
var_dump($baz == $bar);

var_dump($bar > $bar);
var_dump($bar < $bar);
var_dump($bar >= $bar);
var_dump($bar <= $bar);

var_dump($bar > $baz);
var_dump($bar < $baz);
var_dump($bar >= $baz);
var_dump($bar <= $baz);

var_dump($bar > true);
var_dump($bar < true);
var_dump($bar >= true);
var_dump($bar <= true);

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
