--TEST--
Test variations in usage of sqrt()
--INI--
precision = 14
--FILE--
<?php
/* 
 * proto float sqrt(float number)
 * Function is implemented in ext/standard/math.c
*/ 


//Test sqrt with a different input values
echo "*** Testing sqrt() : usage variations ***\n";

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
	$res = sqrt($values[$i]);
	var_dump($res);
}

?>
===Done===
--EXPECTF--
*** Testing sqrt() : usage variations ***
float(4.7958315233127)
float(NAN)
float(4.8425200051213)
float(NAN)
float(4.7958315233127)
float(4.7958315233127)
float(4.7958315233127)
float(4.8425200051213)
float(4.8425200051213)

Warning: sqrt() expects parameter 1 to be float, string given in %s on line %d
NULL
float(31.622776601684)

Notice: A non well formed numeric value encountered in %s on line %d
float(31.622776601684)
float(0)
float(1)
float(0)
===Done===
