--TEST--
Test parse_url() function: Checks relative URL schemes (e.g. "//example.com")
--FILE--
<?php
var_dump(parse_url('//example.org'));
--EXPECT--
array(1) {
  ["host"]=>
  string(11) "example.org"
}
