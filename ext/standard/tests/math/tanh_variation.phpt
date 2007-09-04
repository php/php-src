--TEST--
Test variations in usage of tanh()
--INI--
precision = 10
--FILE--
<?php
/* 
 * proto float tanh(float number)
 * Function is implemented in ext/standard/math.c
*/ 


//Test tanh with a different input values

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
	$res = tanh($values[$i]);
	var_dump($res);
}

?>
--EXPECT--
float(1)
float(-1)
float(1)
float(-1)
float(1)
float(1)
float(1)
float(1)
float(1)
float(0)
float(1)
float(1)
float(0)
float(0.761594156)
float(0)
