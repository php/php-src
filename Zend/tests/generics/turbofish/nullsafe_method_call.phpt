--TEST--
Turbofish: nullsafe method call
--FILE--
<?php
class C {
    function m<T>($x) { return $x; }
}
$c = new C;
var_dump($c?->m::<int>(5));
$null = null;
var_dump($null?->m::<int>(5));
?>
--EXPECT--
int(5)
NULL
