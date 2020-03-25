--TEST--
"Cast during unpacking - non existing values" list()
--FILE--
<?php
$arr = [];

@list((string) $a, (bool) $b, (float) $c, $d) = $arr;
var_dump($a, $b, $c, $d);

@[(string) $a, (bool) $b, (float) $c, $d] = $arr;
var_dump($a, $b, $c, $d);
?>
--EXPECT--
string(0) ""
bool(false)
float(0)
NULL
string(0) ""
bool(false)
float(0)
NULL
