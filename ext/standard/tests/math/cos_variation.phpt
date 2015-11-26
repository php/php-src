--TEST--
Test variations in usage of cos()
--INI--
precision = 10
--FILE--
<?php
/* 
 * proto float cos(float number)
 * Function is implemented in ext/standard/math.c
*/ 


//Test cos with a different input values

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
	$res = cos($values[$i]);
	var_dump($res);
}

?>
--EXPECTF--
float(-0.5328330203)
float(-0.5328330203)
float(-0.1117112391)
float(-0.1117112391)
float(-0.5328330203)
float(-0.5328330203)
float(-0.5328330203)
float(-0.1117112391)
float(-0.1117112391)

Warning: cos() expects parameter 1 to be float, string given in %s on line %d
NULL
float(0.5623790763)

Notice: A non well formed numeric value encountered in %s on line %d
float(0.5623790763)
float(1)
float(0.5403023059)
float(1)
