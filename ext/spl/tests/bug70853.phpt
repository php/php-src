--TEST--
Bug #70853 (SplFixedArray throws exception when using ref variable as index)
--FILE--
<?php

$list = new SplFixedArray(10);
$ndx = 1;
$ndx2 =& $ndx;
$list[$ndx] = 123;	// This throws an exception;
$list[$ndx2] = 123;	// as does this, to.
echo 'ok';

?>
--EXPECT--
ok
