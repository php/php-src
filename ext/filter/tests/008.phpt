--TEST--
filter_list()
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

var_dump(filter_list());
var_dump(filter_list(array()));

echo "Done\n";
?>
--EXPECTF--	
array(20) {
  [0]=>
  string(3) "int"
  [1]=>
  string(7) "boolean"
  [2]=>
  string(5) "float"
  [3]=>
  string(15) "validate_regexp"
  [4]=>
  string(12) "validate_url"
  [5]=>
  string(14) "validate_email"
  [6]=>
  string(11) "validate_ip"
  [7]=>
  string(12) "validate_mac"
  [8]=>
  string(6) "string"
  [9]=>
  string(8) "stripped"
  [10]=>
  string(7) "encoded"
  [11]=>
  string(13) "special_chars"
  [12]=>
  string(18) "full_special_chars"
  [13]=>
  string(10) "unsafe_raw"
  [14]=>
  string(5) "email"
  [15]=>
  string(3) "url"
  [16]=>
  string(10) "number_int"
  [17]=>
  string(12) "number_float"
  [18]=>
  string(12) "magic_quotes"
  [19]=>
  string(8) "callback"
}

Warning: filter_list() expects exactly 0 parameters, 1 given in %s on line %d
NULL
Done
