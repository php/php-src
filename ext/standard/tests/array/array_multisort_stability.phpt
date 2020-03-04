--TEST--
array_multisort() is stable
--FILE--
<?php

// Something of a dummy example where 0 and '0' are used as equal elements.

$ary1 = array_fill(0, 100, 0);
$origAry2 = $ary2 = array_merge(...array_fill(0, 50, [0, '0']));
array_multisort($ary1, $ary2);
var_dump($ary2 === $origAry2);

?>
--EXPECT--
bool(true)
