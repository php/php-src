--TEST--
Test variations in usage of log2()
--INI--
precision = 10
--FILE--
<?php
/* 
 * proto float log2(float number)
 * Function is implemented in ext/standard/math.c
*/ 


//Test log2 with a different input values

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
	$res = log2($values[$i]);
	var_dump($res);
}

?>
--EXPECTF--
float(4.523561956)
float(NAN)
float(4.551516018)
float(NAN)
float(4.523561956)
float(4.523561956)
float(4.523561956)
float(4.551516018)
float(4.551516018)

Warning: log2() expects parameter 1 to be double, string given in %s on line %d
NULL
float(9.965784285)

Notice: A non well formed numeric value encountered in %s on line %d
float(9.965784285)
float(-INF)
float(0)
float(-INF)
