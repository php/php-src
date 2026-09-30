--TEST--
Array-valued INI entries are ignored where a string is expected
--INI--
precision[]=5
array_value[]=1
--FILE--
<?php
var_dump(ini_get('precision'));
var_dump(parse_ini_string('a = ${array_value}'));
?>
--EXPECT--
string(2) "14"
array(1) {
  ["a"]=>
  string(0) ""
}
