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
bool(false)

Warning: Illegal string offset 'foo' in %soffsets_chaining_5.php on line %d
string(1) "f"
bool(false)

Warning: Illegal string offset 'foo' in %soffsets_chaining_5.php on line %d

Warning: Illegal string offset 'bar' in %soffsets_chaining_5.php on line %d
string(1) "f"
