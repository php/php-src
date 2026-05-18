--TEST--
Generic syntax: turbofish on function call
--FILE--
<?php
function add<A, B = mixed, C = mixed>($a, $b) { return $a + $b; }
echo add::<int>(1, 2), "\n";
echo add::<int, int>(3, 4), "\n";
echo add::<int, int, int>(5, 6), "\n";
?>
--EXPECT--
3
7
11
