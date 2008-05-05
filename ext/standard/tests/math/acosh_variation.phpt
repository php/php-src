--TEST--
Test variations in usage of acosh()
--INI--
precision = 10
--FILE--
<?php
/* 
 * proto float acosh(float number)
 * Function is implemented in ext/standard/math.c
*/ 


//Test acosh with a different input values

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
	$res = acosh($values[$i]);
	var_dump($res);
}

?>
--EXPECTF--
float(3.828168471)
float(NAN)
float(3.847562739)
float(NAN)
float(3.828168471)
float(3.828168471)
float(3.828168471)
float(3.847562739)
float(3.847562739)

Warning: acosh() expects parameter 1 to be double, string given in %s on line %d
NULL
float(7.60090221)

Notice: A non well formed numeric value encountered in %s on line %d
float(7.60090221)
float(NAN)
float(0)
float(NAN)
