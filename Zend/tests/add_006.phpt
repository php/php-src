--TEST--
adding numbers to strings
--INI--
precision=14
--FILE--
<?php

$i = 75636;
$s1 = "this is a string";
$s2 = "876222numeric";
$s3 = "48474874";
$s4 = "25.68";

$c = $i + $s1;
var_dump($c);

$c = $i + $s2;
var_dump($c);

$c = $i + $s3;
var_dump($c);

$c = $i + $s4;
var_dump($c);

$c = $s1 + $i;
var_dump($c);

$c = $s2 + $i;
var_dump($c);

$c = $s3 + $i;
var_dump($c);

$c = $s4 + $i;
var_dump($c);

echo "Done\n";
?>
--EXPECTF--	

Warning: A non-numeric value encountered in %s on line %d
int(75636)

Notice: A non well formed numeric value encountered in %s on line %d
int(951858)
int(48550510)
float(75661.68)

Warning: A non-numeric value encountered in %s on line %d
int(75636)

Notice: A non well formed numeric value encountered in %s on line %d
int(951858)
int(48550510)
float(75661.68)
Done
