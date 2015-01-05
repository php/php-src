--TEST--
htmlentities() / html_entity_decode() #8592 - #9002 table test
--FILE--
<?php
$tests = array(
	array(8853, '&oplus;',  "e28a95"),
	array(8855, '&otimes;', "e28a97"),
	array(8869, '&perp;',   "e28aa5"),
	array(8901, '&sdot;',   "e28b85"),
	array(8968, '&lceil;',  "e28c88"),
	array(8969, '&rceil;',  "e28c89"),
	array(8970, '&lfloor;', "e28c8a"),
	array(8971, '&rfloor;', "e28c8b"),
	array(9001, '&lang;',   "e28ca9"),
	array(9002, '&rang;',   "e28caa")
);

foreach ($tests as $test) {
	var_dump(htmlentities(pack('H*', $test[2]), ENT_QUOTES, 'UTF-8'));
}

foreach ($tests as $test) {
	list(,$result) = unpack('H6', html_entity_decode($test[1], ENT_QUOTES, 'UTF-8'));
	var_dump($result);
}
?>
--EXPECT--
string(7) "&oplus;"
string(8) "&otimes;"
string(6) "&perp;"
string(6) "&sdot;"
string(7) "&lceil;"
string(7) "&rceil;"
string(8) "&lfloor;"
string(8) "&rfloor;"
string(6) "&lang;"
string(6) "&rang;"
string(6) "e28a95"
string(6) "e28a97"
string(6) "e28aa5"
string(6) "e28b85"
string(6) "e28c88"
string(6) "e28c89"
string(6) "e28c8a"
string(6) "e28c8b"
string(6) "e28ca9"
string(6) "e28caa"
