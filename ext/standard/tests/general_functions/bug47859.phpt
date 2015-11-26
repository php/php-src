--TEST--
Bug #47859 (parse_ini_file() does not like asterisk (*) in key in the beginning)
--FILE--
<?php
var_dump(parse_ini_string('*key = "*value"'));
var_dump(parse_ini_string('-key = "-value"'));
var_dump(parse_ini_string('_key = "_value"'));

var_dump(parse_ini_string('key* = "value*"'));
var_dump(parse_ini_string('key.*.* = "value.*.*"'));
var_dump(parse_ini_string('*.*.key = "*.*.value"'));
var_dump(parse_ini_string('k*e*y = "v*a*lue"'));
?>
--EXPECT--
array(1) {
  ["*key"]=>
  string(6) "*value"
}
array(1) {
  ["-key"]=>
  string(6) "-value"
}
array(1) {
  ["_key"]=>
  string(6) "_value"
}
array(1) {
  ["key*"]=>
  string(6) "value*"
}
array(1) {
  ["key.*.*"]=>
  string(9) "value.*.*"
}
array(1) {
  ["*.*.key"]=>
  string(9) "*.*.value"
}
array(1) {
  ["k*e*y"]=>
  string(7) "v*a*lue"
}
