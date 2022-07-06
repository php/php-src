--TEST--
Test parse_url() for bug #63162
--DESCRIPTION--
This test covers tests the inputs:
[0]=> http://user:pass@host
[1]=> //user:pass@host
[2]=> //user@host
--FILE--
<?php
var_dump(parse_url('http://user:pass@host'));
var_dump(parse_url('//user:pass@host'));
var_dump(parse_url('//user@host'));
?>
--EXPECT--
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(4) "host"
  ["user"]=>
  string(4) "user"
  ["pass"]=>
  string(4) "pass"
}
array(3) {
  ["host"]=>
  string(4) "host"
  ["user"]=>
  string(4) "user"
  ["pass"]=>
  string(4) "pass"
}
array(2) {
  ["host"]=>
  string(4) "host"
  ["user"]=>
  string(4) "user"
}
