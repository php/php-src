--TEST--
Turbofish: first-class callable on instance method
--FILE--
<?php
class C { function m<T>($x) { return $x * 2; } }
$cl = (new C)->m::<int>(...);
var_dump($cl(5));
?>
--EXPECT--
int(10)
