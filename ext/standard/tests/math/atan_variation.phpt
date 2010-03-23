--TEST--
Test variations in usage of atan()
--INI--
precision = 10
--FILE--
<?php
/* 
 * proto float atan(float number)
 * Function is implemented in ext/standard/math.c
*/ 


//Test atan with a different input values

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
	$res = atan($values[$i]);
	var_dump($res);
}

?>
--EXPECTF--
float(1.527345431)
float(-1.527345431)
float(1.528178225)
float(-1.528178225)
float(1.527345431)
float(1.527345431)
float(1.527345431)
float(1.528178225)
float(1.528178225)

Warning: atan() expects parameter 1 to be double, string given in %s on line %d
NULL
float(1.569796327)

Notice: A non well formed numeric value encountered in %s on line %d
float(1.569796327)
float(0)
float(0.7853981634)
float(0)
