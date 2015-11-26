--TEST--
adding integers to doubles
--INI--
precision=14
--FILE--
<?php

$i = 75636;
$d = 2834681123.123123;

$c = $i + $d;
var_dump($c);

$c = $d + $i;
var_dump($c);

echo "Done\n";
?>
--EXPECTF--	
float(2834756759.1231)
float(2834756759.1231)
Done
