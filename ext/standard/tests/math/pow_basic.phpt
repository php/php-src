--TEST--
Test pow() - basic function test pow()
--INI--
precision=14
--FILE--
<?php
$values = array(23,
				-23,
				2.345e1,
				-2.345e1,
				0x17,
				027,
				"23",
				"23.45",
				"2.345e1",				
				null,
				true,
				false);	

for ($i = 0; $i < count($values); $i++) {
	$res = pow($values[$i], 4);
	var_dump($res);
}
?>
--EXPECTF--
int(279841)
int(279841)
float(302392.75950625)
float(302392.75950625)
int(279841)
int(279841)
int(279841)
float(302392.75950625)
float(302392.75950625)
int(0)
int(1)
int(0)