--TEST--
Turbofish: instanceof discards type arguments
--FILE--
<?php
class B<T> {}
class C {}
$c = new C;
var_dump($c instanceof C);
var_dump($c instanceof C<int>);
var_dump($c instanceof C<int, string, B<int>>);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
