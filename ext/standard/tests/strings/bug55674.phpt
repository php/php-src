--TEST--
Bug #55674 (fgetcsv & str_getcsv skip empty fields in some tab-separated records)
--FILE--
<?php
var_dump(str_getcsv("0\t\t\"2\"\n", "\t"));
var_dump(str_getcsv("0\t \t'2'\n", "\t", "'"));
var_dump(str_getcsv(",,,,"));
var_dump(str_getcsv(" \t  \t\t\t ", "\t"));
?>
--EXPECT--
array(3) {
  [0]=>
  string(1) "0"
  [1]=>
  string(0) ""
  [2]=>
  string(1) "2"
}
array(3) {
  [0]=>
  string(1) "0"
  [1]=>
  string(1) " "
  [2]=>
  string(1) "2"
}
array(5) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
  [4]=>
  string(0) ""
}
array(5) {
  [0]=>
  string(1) " "
  [1]=>
  string(2) "  "
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
  [4]=>
  string(1) " "
}
