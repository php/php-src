--TEST--
Bug #54180 (parse_url() incorrectly parses path when ? in fragment)
--FILE--
<?php

var_dump(parse_url("http://example.com/path/script.html?t=1#fragment?data"));
var_dump(parse_url("http://example.com/path/script.html#fragment?data"));

?>
--EXPECTF--
array(5) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "example.com"
  ["path"]=>
  string(17) "/path/script.html"
  ["query"]=>
  string(3) "t=1"
  ["fragment"]=>
  string(13) "fragment?data"
}
array(4) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "example.com"
  ["path"]=>
  string(17) "/path/script.html"
  ["fragment"]=>
  string(13) "fragment?data"
}
