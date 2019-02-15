--TEST--
parse_ini_file() boolean operators
--FILE--
<?php

$ini_file = dirname(__FILE__)."/parse_ini_booleans.data";

var_dump(parse_ini_file($ini_file, 1));

echo "Done.\n";

?>
--EXPECT--
array(3) {
  ["error_reporting values"]=>
  array(6) {
    ["foo"]=>
    string(7) "32767 8"
    ["error_reporting"]=>
    string(5) "32767"
    ["error_reporting1"]=>
    string(4) "4177"
    ["error_reporting2"]=>
    string(5) "32759"
    ["error_reporting3"]=>
    string(5) "32759"
    ["error_reporting4"]=>
    string(5) "32759"
  }
  ["true or false"]=>
  array(8) {
    ["bool_true"]=>
    string(1) "1"
    ["bool_yes"]=>
    string(1) "1"
    ["bool_on"]=>
    string(1) "1"
    ["bool_false"]=>
    string(0) ""
    ["bool_off"]=>
    string(0) ""
    ["bool_no"]=>
    string(0) ""
    ["bool_none"]=>
    string(0) ""
    ["bool_null"]=>
    string(0) ""
  }
  ["strings"]=>
  array(8) {
    ["string_true"]=>
    string(4) "true"
    ["string_yes"]=>
    string(4) " yes"
    ["string_on"]=>
    string(5) "  on "
    ["string_false"]=>
    string(5) "false"
    ["string_off"]=>
    string(4) "Off "
    ["string_no"]=>
    string(4) "No	 "
    ["string_none"]=>
    string(5) " NoNe"
    ["string_null"]=>
    string(4) "NULl"
  }
}
Done.
