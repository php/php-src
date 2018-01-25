--TEST--
Test decoct() - basic function test decoct()
--FILE--
<?php
$values = array(10,
				3950.5,
				3.9505e3,
				03,
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
string(2) "12"
string(4) "7556"
string(4) "7556"
string(1) "3"
string(3) "137"
string(2) "12"
string(4) "7556"
string(4) "7556"
string(2) "47"
string(1) "0"
string(1) "1"
string(1) "0"
string(1) "0"
