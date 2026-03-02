--TEST--
GH-11010: Preserve ini formatting of concatenated numbers
--FILE--
<?php
$result = parse_ini_string('variable = -00 20 30', false, INI_SCANNER_TYPED);
var_dump($result['variable']);
?>
--EXPECT--
string(9) "-00 20 30"
