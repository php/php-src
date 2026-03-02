--TEST--
filter_list()
--EXTENSIONS--
filter
--FILE--
<?php

var_dump(filter_list());

echo "Done\n";
?>
--EXPECT--
array(21) {
  [0]=>
  string(3) "int"
  [1]=>
  string(7) "boolean"
  [2]=>
  string(5) "float"
  [3]=>
  string(15) "validate_regexp"
  [4]=>
  string(15) "validate_domain"
  [5]=>
  string(12) "validate_url"
  [6]=>
  string(14) "validate_email"
  [7]=>
  string(11) "validate_ip"
  [8]=>
  string(12) "validate_mac"
  [9]=>
  string(6) "string"
  [10]=>
  string(8) "stripped"
  [11]=>
  string(7) "encoded"
  [12]=>
  string(13) "special_chars"
  [13]=>
  string(18) "full_special_chars"
  [14]=>
  string(10) "unsafe_raw"
  [15]=>
  string(5) "email"
  [16]=>
  string(3) "url"
  [17]=>
  string(10) "number_int"
  [18]=>
  string(12) "number_float"
  [19]=>
  string(11) "add_slashes"
  [20]=>
  string(8) "callback"
}
Done
