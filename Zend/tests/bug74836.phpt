--TEST--
Bug #74836 (isset on zero-prefixed numeric indexes in array broken)
--FILE--
<?php
$s = "1234567890a";
$a[10] = "42";
$i = "010";

var_dump($s["10"], isset($s["10"]));
var_dump($s["010"], isset($s["010"]));
var_dump($s[$i], isset($s[$i]));

var_dump($a["10"], isset($a["10"]));
var_dump($a["010"], isset($a["010"]));
var_dump($a[$i], isset($a[$i]));
?>
--EXPECTF--
string(1) "a"
bool(true)
string(1) "a"
bool(true)
string(1) "a"
bool(true)
string(2) "42"
bool(true)

Warning: Undefined array key "010" in %s on line %d
NULL
bool(false)

Warning: Undefined array key "010" in %s on line %d
NULL
bool(false)
