--TEST--
Test variations in usage of sin()
--INI--
precision = 10
--FILE--
<?php
/*
 * proto float sin(float number)
 * Function is implemented in ext/standard/math.c
*/


//Test sin with a different input values

$values = array(23,
		-23,
		2.345e1,
		-2.345e1,
		0x17,
		027,
		"23",
		"23.45",
		"2.345e1",
		"nonsense",
		"1000",
		"1000ABC",
		null,
		true,
		false);

for ($i = 0; $i < count($values); $i++) {
	$res = sin($values[$i]);
	var_dump($res);
}

?>
--EXPECTF--
float(-0.8462204042)
float(0.8462204042)
float(-0.9937407102)
float(0.9937407102)
float(-0.8462204042)
float(-0.8462204042)
float(-0.8462204042)
float(-0.9937407102)
float(-0.9937407102)

Warning: sin() expects parameter 1 to be float, string given in %s on line %d
NULL
float(0.8268795405)

Notice: A non well formed numeric value encountered in %s on line %d
float(0.8268795405)
float(0)
float(0.8414709848)
float(0)
