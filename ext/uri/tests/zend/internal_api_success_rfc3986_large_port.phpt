--TEST--
Test zend_test_uri_parser() - success - large RFC 3986 port
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(zend_test_uri_parser("https://example.com:42424242", "Uri\\Rfc3986\\Uri"));

?>
--EXPECT--
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
    int(42424242)
    ["path"]=>
    string(0) ""
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
    int(42424242)
    ["path"]=>
    string(0) ""
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
    int(42424242)
    ["path"]=>
    string(0) ""
    ["query"]=>
    NULL
    ["fragment"]=>
    NULL
  }
}
