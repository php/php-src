--TEST--
Fixed bug #51094 (parse_ini_file() with INI_SCANNER_RAW cuts a value that includes a semi-colon).
--FILE--
<?php

$ini = parse_ini_string('ini="ini;raw"', false, INI_SCANNER_RAW);
var_dump($ini['ini']);
$ini = parse_ini_string('ini="ini;raw', false, INI_SCANNER_RAW);
var_dump($ini['ini']);
$ini = parse_ini_string('ini=ini;raw', false, INI_SCANNER_RAW);
var_dump($ini['ini']);
$ini = parse_ini_string('ini=ini"raw', false, INI_SCANNER_RAW);
var_dump($ini['ini']);
$ini = parse_ini_string("ini=\r\niniraw", false, INI_SCANNER_RAW);
var_dump($ini['ini']);
?>
--EXPECT--
string(7) "ini;raw"
string(4) ""ini"
string(3) "ini"
string(7) "ini"raw"
string(0) ""
