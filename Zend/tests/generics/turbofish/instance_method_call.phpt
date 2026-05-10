--TEST--
Turbofish: instance method call
--FILE--
<?php
class C {
    function m<T>($x) { return $x; }
}
$c = new C;
var_dump($c->m::<int>(99));
?>
--EXPECT--
int(99)
