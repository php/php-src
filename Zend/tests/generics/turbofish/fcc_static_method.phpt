--TEST--
Turbofish: first-class callable on static method
--FILE--
<?php
class C { static function m<T>($x) { return $x; } }
$cl = C::m::<int>(...);
var_dump($cl(11));
?>
--EXPECT--
int(11)
