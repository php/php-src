--TEST--
Generic syntax: turbofish on instance method call (including nullsafe)
--FILE--
<?php
class C {
    public function m<T>($x) { return $x * 3; }
}
$c = new C;
echo $c->m::<int>(5), "\n";
echo $c?->m::<int>(7), "\n";
$null = null;
var_dump($null?->m::<int>(9));
?>
--EXPECT--
15
21
NULL
