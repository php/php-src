--TEST--
Test zend_test_uri_parser() - success - parse_url-based absolute URI without path
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(zend_test_uri_parser('https://example.com', 'parse_url'));

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
