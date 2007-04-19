--TEST--
double to string conversion tests
--FILE--
<?php

$doubles = array(
	290000000000000000,
	290000000000000,
	29000000000000,
	29000000000000.123123,
	29000000000000.7123123,
	29000.7123123,
	239234242.7123123,
	0.12345678901234567890,
	10000000000000,
	100000000000000,
	1000000000000000001,
	100000000000001,
	10000000000,
	999999999999999,
	9999999999999999,
	(float)0
	);

foreach ($doubles as $d) {
	var_dump((string)$d);
}

echo "Done\n";
?>
--EXPECTF--	
string(7) "2.9E+17"
string(7) "2.9E+14"
string(14) "29000000000000"
string(14) "29000000000000"
string(14) "29000000000001"
string(13) "29000.7123123"
string(15) "239234242.71231"
string(16) "0.12345678901235"
string(14) "10000000000000"
string(7) "1.0E+14"
string(7) "1.0E+18"
string(7) "1.0E+14"
string(11) "10000000000"
string(7) "1.0E+15"
string(7) "1.0E+16"
string(1) "0"
Done
--UEXPECTF--
unicode(7) "2.9E+17"
unicode(7) "2.9E+14"
unicode(14) "29000000000000"
unicode(14) "29000000000000"
unicode(14) "29000000000001"
unicode(13) "29000.7123123"
unicode(15) "239234242.71231"
unicode(16) "0.12345678901235"
unicode(14) "10000000000000"
unicode(7) "1.0E+14"
unicode(7) "1.0E+18"
unicode(7) "1.0E+14"
unicode(11) "10000000000"
unicode(7) "1.0E+15"
unicode(7) "1.0E+16"
unicode(1) "0"
Done
