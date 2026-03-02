--TEST--
Test that return value handling for DateInterval property writes do not corrupt RHS
--FILE--
<?php

$interval = new DateInterval('P2Y4DT6H8M');
$f = 0.5;
var_dump($interval->f = $f);
var_dump($f);

?>
--EXPECT--
float(0.5)
float(0.5)
