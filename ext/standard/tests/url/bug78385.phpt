--TEST--
Bug #78385 parse_url() does not include 'query' when question mark is the last char
--FILE--
<?php

var_dump(parse_url('http://example.com/foo?'));
var_dump(parse_url('http://example.com/foo?', PHP_URL_QUERY));

?>
--EXPECT--
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "example.com"
  ["path"]=>
  string(4) "/foo"
  ["query"]=>
  string(0) ""
}
string(0) ""
