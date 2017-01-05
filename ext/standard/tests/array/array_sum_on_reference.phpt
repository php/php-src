--TEST--
array_sum() on array with references
--FILE--
<?php

$n = "10";
$n .= "0";
$nums = [&$n, 100];
var_dump(array_sum($nums));
var_dump($n);

?>
--EXPECT--
int(200)
string(3) "100"
