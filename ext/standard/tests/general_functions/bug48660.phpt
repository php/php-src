--TEST--
Bug #48660 (parse_ini_*(): dollar sign as last character of value fails)
--FILE--
<?php

$ini_location = dirname(__FILE__) . '/bug48660.tmp';

// Build ini data
$ini_data = '
[cases]

Case.a = avalue
Case.b = "$dollar_sign"
Case.c = "dollar_sign$"
Case.d = "$dollar_sign$"
Case.e = 10
';

// Save ini data to file
file_put_contents($ini_location, $ini_data);

var_dump(parse_ini_file($ini_location, true, INI_SCANNER_RAW));
var_dump(parse_ini_file($ini_location, true, INI_SCANNER_NORMAL));

?>
--CLEAN--
<?php @unlink(dirname(__FILE__) . '/bug48660.tmp'); ?>
--EXPECTF--
array(1) {
  ["cases"]=>
  array(5) {
    ["Case.a"]=>
    string(6) "avalue"
    ["Case.b"]=>
    string(12) "$dollar_sign"
    ["Case.c"]=>
    string(12) "dollar_sign$"
    ["Case.d"]=>
    string(13) "$dollar_sign$"
    ["Case.e"]=>
    string(2) "10"
  }
}
array(1) {
  ["cases"]=>
  array(5) {
    ["Case.a"]=>
    string(6) "avalue"
    ["Case.b"]=>
    string(12) "$dollar_sign"
    ["Case.c"]=>
    string(12) "dollar_sign$"
    ["Case.d"]=>
    string(13) "$dollar_sign$"
    ["Case.e"]=>
    string(2) "10"
  }
}
