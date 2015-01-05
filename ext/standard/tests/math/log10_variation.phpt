--TEST--
Test variations in usage of log10()
--INI--
precision = 10
--FILE--
<?php
/* 
 * proto float log10(float number)
 * Function is implemented in ext/standard/math.c
*/ 


//Test log10 with a different input values

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
	$res = log10($values[$i]);
	var_dump($res);
}

?>
--EXPECTF--
float(1.361727836)
float(NAN)
float(1.370142847)
float(NAN)
float(1.361727836)
float(1.361727836)
float(1.361727836)
float(1.370142847)
float(1.370142847)

Warning: log10() expects parameter 1 to be float, string given in %s on line %d
NULL
float(3)

Notice: A non well formed numeric value encountered in %s on line %d
float(3)
float(-INF)
float(0)
float(-INF)
