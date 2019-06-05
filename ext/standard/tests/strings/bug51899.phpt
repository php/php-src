--TEST--
Bug #51899 (Parse error in parse_ini_file() function when empy value followed by no newline)
--FILE--
<?php

var_dump(parse_ini_string('a='));
var_dump(parse_ini_string('a= '));
var_dump(parse_ini_string('a='.PHP_EOL));
var_dump(parse_ini_string('a=b '));
var_dump(parse_ini_string(''));
var_dump(parse_ini_string(NULL));
var_dump(parse_ini_string("\0"));

?>
--EXPECTF--
array(1) {
  ["a"]=>
  string(0) ""
}
array(1) {
  ["a"]=>
  string(0) ""
}
array(1) {
  ["a"]=>
  string(0) ""
}
array(1) {
  ["a"]=>
  string(2) "b "
}
array(0) {
}

Deprecated: Passing null to argument of type string is deprecated in %s on line %d
array(0) {
}
array(0) {
}
