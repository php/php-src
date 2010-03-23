--TEST--
Test variations in usage of deg2rad()
--INI--
precision = 10
--FILE--
<?php
/* 
 * proto float deg2rad(float number)
 * Function is implemented in ext/standard/math.c
*/ 


//Test deg2rad with a different input values

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
	$res = deg2rad($values[$i]);
	var_dump($res);
}

?>
--EXPECTF--
float(0.401425728)
float(-0.401425728)
float(0.4092797096)
float(-0.4092797096)
float(0.401425728)
float(0.401425728)
float(0.401425728)
float(0.4092797096)
float(0.4092797096)

Warning: deg2rad() expects parameter 1 to be double, string given in %s on line %d
NULL
float(17.45329252)

Notice: A non well formed numeric value encountered in %s on line %d
float(17.45329252)
float(0)
float(0.01745329252)
float(0)
