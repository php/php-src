--TEST--
Test zend_test_uri_parser() with parse_url missing components
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(zend_test_uri_parser('/foo?bar#baz', 'parse_url'));
var_dump(zend_test_uri_parser('https://example.com', 'parse_url'));

?>
--EXPECT--
array(3) {
  ["normalized"]=>
  array(8) {
    ["scheme"]=>
    NULL
    ["username"]=>
    NULL
    ["password"]=>
    NULL
    ["host"]=>
    NULL
    ["port"]=>
    NULL
    ["path"]=>
    string(4) "/foo"
    ["query"]=>
    string(3) "bar"
    ["fragment"]=>
    string(3) "baz"
  }
  ["raw"]=>
  array(8) {
    ["scheme"]=>
    NULL
    ["username"]=>
    NULL
    ["password"]=>
    NULL
    ["host"]=>
    NULL
    ["port"]=>
    NULL
    ["path"]=>
    string(4) "/foo"
    ["query"]=>
    string(3) "bar"
    ["fragment"]=>
    string(3) "baz"
  }
  ["struct"]=>
  array(8) {
    ["scheme"]=>
    NULL
    ["username"]=>
    NULL
    ["password"]=>
    NULL
    ["host"]=>
    NULL
    ["port"]=>
    int(0)
    ["path"]=>
    string(4) "/foo"
    ["query"]=>
    string(3) "bar"
    ["fragment"]=>
    string(3) "baz"
  }
}
array(3) {
  ["normalized"]=>
  array(8) {
    ["scheme"]=>
    string(5) "https"
    ["username"]=>
    NULL
    ["password"]=>
    NULL
    ["host"]=>
    string(11) "example.com"
    ["port"]=>
    NULL
    ["path"]=>
    NULL
    ["query"]=>
    NULL
    ["fragment"]=>
    NULL
  }
  ["raw"]=>
  array(8) {
    ["scheme"]=>
    string(5) "https"
    ["username"]=>
    NULL
    ["password"]=>
    NULL
    ["host"]=>
    string(11) "example.com"
    ["port"]=>
    NULL
    ["path"]=>
    NULL
    ["query"]=>
    NULL
    ["fragment"]=>
    NULL
  }
  ["struct"]=>
  array(8) {
    ["scheme"]=>
    string(5) "https"
    ["username"]=>
    NULL
    ["password"]=>
    NULL
    ["host"]=>
    string(11) "example.com"
    ["port"]=>
    int(0)
    ["path"]=>
    NULL
    ["query"]=>
    NULL
    ["fragment"]=>
    NULL
  }
}
