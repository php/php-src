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
array(23) {
  [0]=>
  string(7) "invalid"
  [1]=>
  string(3) "int"
  [2]=>
  string(7) "boolean"
  [3]=>
  string(5) "float"
  [4]=>
  string(15) "validate_string"
  [5]=>
  string(15) "validate_regexp"
  [6]=>
  string(15) "validate_domain"
  [7]=>
  string(12) "validate_url"
  [8]=>
  string(14) "validate_email"
  [9]=>
  string(11) "validate_ip"
  [10]=>
  string(12) "validate_mac"
  [11]=>
  string(6) "string"
  [12]=>
  string(8) "stripped"
  [13]=>
  string(7) "encoded"
  [14]=>
  string(13) "special_chars"
  [15]=>
  string(18) "full_special_chars"
  [16]=>
  string(10) "unsafe_raw"
  [17]=>
  string(5) "email"
  [18]=>
  string(3) "url"
  [19]=>
  string(10) "number_int"
  [20]=>
  string(12) "number_float"
  [21]=>
  string(12) "magic_quotes"
  [22]=>
  string(8) "callback"
}

Warning: filter_list() expects exactly 0 parameters, 1 given in %s on line %d
NULL
Done
