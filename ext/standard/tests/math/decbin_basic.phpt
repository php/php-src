--TEST--
Test decbin() - basic function test
--FILE--
<?php
$values = array(10,
				3950.5,
				3.9505e3,
				039,
				0x5F,	
				"10",
				"3950.5",
				"3.9505e3",
				"039",
				"0x5F",
				true,
				false,
				null, 
				);	

for ($i = 0; $i < count($values); $i++) {
	$res = decbin($values[$i]);
	var_dump($res);
}
?>

--EXPECTF--
string(4) "1010"
string(12) "111101101110"
string(12) "111101101110"
string(2) "11"
string(7) "1011111"
string(4) "1010"
string(12) "111101101110"
string(2) "11"
string(6) "100111"
string(1) "0"
string(1) "1"
string(1) "0"
string(1) "0"
--UEXPECTF--
unicode(4) "1010"
unicode(12) "111101101110"
unicode(12) "111101101110"
unicode(2) "11"
unicode(7) "1011111"
unicode(4) "1010"
unicode(12) "111101101110"
unicode(2) "11"
unicode(6) "100111"
unicode(1) "0"
unicode(1) "1"
unicode(1) "0"
unicode(1) "0"
