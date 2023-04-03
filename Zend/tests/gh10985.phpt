--TEST--
GH-10985: instanceof with class const in rhs
--FILE--
<?php
class Foo {
    const BAR = 'stdClass';
}
$c = new stdClass();
var_dump($c instanceof (stdClass::class));
var_dump($c instanceof stdClass::class);
$bar = 'BAR';
var_dump($c instanceof Foo::{$bar});
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
