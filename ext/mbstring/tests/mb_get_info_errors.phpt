--TEST--
mb_get_info() errors
--EXTENSIONS--
mbstring
--INI--
--FILE--
<?php
var_dump(mb_get_info("http_input"));
var_dump(mb_get_info("detect_order"));
var_dump(mb_get_info("non_sense"));
?>
--EXPECTF--
NULL
array(2) {
  [0]=>
  string(5) "ASCII"
  [1]=>
  string(5) "UTF-8"
}

Warning: mb_get_info(): argument #1 ($type) must be a valid type in %s on line %d
bool(false)
