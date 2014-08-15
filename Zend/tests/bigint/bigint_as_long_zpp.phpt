--TEST--
Bounds check on large bigints passed to functions expecting longs
--FILE--
<?php

// This will be well beyond the range of a 32-bit or 64-bit integer
$n = 2 ** 128;

// rand() doesn't support bigint values
rand(0, $n);
?>
--EXPECTF--
Catchable fatal error: rand() expects parameter 2 to be a %d-bit integer (within the range -%d to %d inclusive), integer given was too large in %s on line 7