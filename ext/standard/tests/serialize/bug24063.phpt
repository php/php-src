--TEST--
Bug #24063 (serialize() missing 0 after the . on scientific notation)
--INI--
serialize_precision=100
precision=12
--FILE--
<?php 
$f = 1.0e-6;
$s = serialize($f);
var_dump($s, unserialize($s));
?>
--EXPECT--
string(9) "d:1.0E-6;"
float(1.0E-6)
