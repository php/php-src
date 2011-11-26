--TEST--
testing the behavior of string offset chaining
--INI--
error_reporting=E_ALL | E_DEPRECATED
--FILE--
<?php
$array = array('expected_array' => "foobar");
var_dump(isset($array['expected_array']));
var_dump($array['expected_array']);
var_dump(isset($array['expected_array']['foo']));
var_dump($array['expected_array']['foo']);
var_dump(isset($array['expected_array']['foo']['bar']));
var_dump($array['expected_array']['foo']['bar']);
?>
--EXPECTF--
bool(true)
string(6) "foobar"
bool(true)
string(1) "f"
bool(false)

Fatal error: Cannot use string offset as an array in %s on line %d

