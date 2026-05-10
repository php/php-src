--TEST--
Generic syntax: turbofish with composite type arguments
--FILE--
<?php
function f<A, B = mixed>($x) { return $x; }
echo f::<int|string>(42), "\n";
echo f::<int|string, (Throwable&Stringable)|null>('hi'), "\n";
?>
--EXPECT--
42
hi
