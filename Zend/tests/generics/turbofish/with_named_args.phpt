--TEST--
Turbofish: combined with named arguments
--FILE--
<?php
function f<A, B = mixed>($a, $b) { return "$a:$b"; }
echo f::<int>(a: 1, b: 2), "\n";
echo f::<int, string>(b: "x", a: 5), "\n";
?>
--EXPECT--
1:2
5:x
