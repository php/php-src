--TEST--
Generic syntax: instanceof with type arguments (args discarded at runtime)
--FILE--
<?php
class C {}
$c = new C;
var_dump($c instanceof C<int>);
var_dump($c instanceof C<int, string>);
$x = new stdClass;
var_dump($x instanceof C<int>);
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
