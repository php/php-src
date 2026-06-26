--TEST--
Reflection: ReflectionFunction::isGeneric()
--FILE--
<?php
function gen<T>(): void {}
function plain(): void {}
var_dump((new ReflectionFunction('gen'))->isGeneric());
var_dump((new ReflectionFunction('plain'))->isGeneric());
?>
--EXPECT--
bool(true)
bool(false)
