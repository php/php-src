--TEST--
Bug #64604 parse_url is inconsistent with specified port
--FILE--
<?php
var_dump(parse_url('//localhost/path'));
var_dump(parse_url('//localhost:80/path'));
var_dump(parse_url('//localhost:/path'));
var_dump(parse_url('http://localhost:80/path'));
?>
--EXPECT--
array(2) {
  ["host"]=>
  string(9) "localhost"
  ["path"]=>
  string(5) "/path"
}
array(3) {
  ["host"]=>
  string(9) "localhost"
  ["port"]=>
  int(80)
  ["path"]=>
  string(5) "/path"
}
array(2) {
  ["host"]=>
  string(9) "localhost"
  ["path"]=>
  string(5) "/path"
}
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(9) "localhost"
  ["port"]=>
  int(80)
  ["path"]=>
  string(5) "/path"
}
