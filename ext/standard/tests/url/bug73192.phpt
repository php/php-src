--TEST--
Bug #73192: parse_url return wrong hostname
--FILE--
<?php

var_dump(parse_url("http://example.com:80#@google.com/"));
var_dump(parse_url("http://example.com:80?@google.com/"));

?>
--EXPECT--
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  int(80)
  ["fragment"]=>
  string(12) "@google.com/"
}
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  int(80)
  ["query"]=>
  string(12) "@google.com/"
}
