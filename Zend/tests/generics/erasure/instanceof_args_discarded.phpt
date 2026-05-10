--TEST--
Erasure: instanceof type arguments discarded at runtime
--FILE--
<?php
class C {}
$c = new C;
var_dump($c instanceof C);
var_dump($c instanceof C<int>);
var_dump($c instanceof C<string, int>);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
