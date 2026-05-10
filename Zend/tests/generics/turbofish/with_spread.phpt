--TEST--
Turbofish: combined with spread/unpacking
--FILE--
<?php
function f<T>($a, $b, $c) { return $a + $b + $c; }
$args = [1, 2, 3];
echo f::<int>(...$args), "\n";
?>
--EXPECT--
6
