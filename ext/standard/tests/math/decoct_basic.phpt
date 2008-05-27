--TEST--
Test decoct() - basic function test decoct()
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
	$res = decoct($values[$i]);
	var_dump($res);
}
?>
--EXPECTF--
unicode(2) "12"
unicode(4) "7556"
unicode(4) "7556"
unicode(1) "3"
unicode(3) "137"
unicode(2) "12"
unicode(4) "7556"
unicode(1) "3"
unicode(2) "47"
unicode(1) "0"
unicode(1) "1"
unicode(1) "0"
unicode(1) "0"
