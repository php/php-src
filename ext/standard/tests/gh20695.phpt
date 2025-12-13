--TEST--
GH-20695 (Assertion failure in normalize_value() when parsing malformed INI input via parse_ini_string())
--FILE--
<?php
var_dump(parse_ini_string('8 [[] = !!$]', true, INI_SCANNER_TYPED));
?>
--EXPECT--
array(1) {
  [8]=>
  array(1) {
    ["["]=>
    int(0)
  }
}
