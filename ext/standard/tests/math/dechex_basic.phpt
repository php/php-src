--TEST--
Test dechex() - basic function dechex()
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
	$res = dechex($values[$i]);
	var_dump($res);
}
?>
--EXPECTF--
unicode(1) "a"
unicode(3) "f6e"
unicode(3) "f6e"
unicode(1) "3"
unicode(2) "5f"
unicode(1) "a"
unicode(3) "f6e"
unicode(1) "3"
unicode(2) "27"
unicode(1) "0"
unicode(1) "1"
unicode(1) "0"
unicode(1) "0"
