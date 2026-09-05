--TEST--
Test zend_test_uri_parser() - success - parse_url-based relative reference
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(zend_test_uri_parser('/foo?bar#baz', 'parse_url'));

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
