--TEST--
Generic syntax: trailing comma in type parameter list
--FILE--
<?php
class Box<T,> {}
class Pair<A, B,> {}
echo (new ReflectionClass('Box'))->isGeneric() ? 'ok' : 'fail', "\n";
echo (new ReflectionClass('Pair'))->isGeneric() ? 'ok' : 'fail', "\n";
?>
--EXPECT--
ok
ok
