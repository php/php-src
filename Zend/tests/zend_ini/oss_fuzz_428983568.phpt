--TEST--
OSS-Fuzz #428983568
--FILE--
<?php
$ini = <<<INI
[\${zz:-x

INI;
var_dump(parse_ini_string($ini));
?>
--EXPECTF--
Warning: syntax error, unexpected end of file, expecting '}' in Unknown on line 1
 in %s on line %d
bool(false)
