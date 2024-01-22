--TEST--
GH-11982 (str_getcsv returns null byte for unterminated quoted string)
--FILE--
<?php
var_dump(str_getcsv('"'));
var_dump(str_getcsv('"field","'));
var_dump(str_getcsv('"","a'));
?>
--EXPECT--
array(1) {
  [0]=>
  string(0) ""
}
array(2) {
  [0]=>
  string(5) "field"
  [1]=>
  string(0) ""
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(1) "a"
}

