--TEST--
Bug #29883 (isset gives invalid values on strings)
--FILE--
<?php
$x = "bug";
var_dump(isset($x[-10]));
var_dump(isset($x[1]));
var_dump(isset($x["1"]));
var_dump($x[-10])."\n";
var_dump($x[1])."\n";
var_dump($x["1"])."\n";
?>
--EXPECTF--
bool(false)
bool(true)
bool(true)

Warning: Uninitialized string offset -10 in %s on line %d
string(0) ""
string(1) "u"
string(1) "u"
