--TEST--
Test the parse_url-based URI parser
--EXTENSIONS--
uri
zend_test
--FILE--
<?php

var_dump(zend_test_uri_parser("https://%65xample:%65xample@%65xample.com:123/%65xample.html?%65xample=%65xample#%65xample", "parse_url"));

?>
--EXPECT--
array(3) {
  ["normalized"]=>
  array(8) {
    ["scheme"]=>
    string(5) "https"
    ["username"]=>
    string(7) "example"
    ["password"]=>
    string(7) "example"
    ["host"]=>
    string(11) "example.com"
    ["port"]=>
    int(123)
    ["path"]=>
    string(13) "/example.html"
    ["query"]=>
    string(15) "example=example"
    ["fragment"]=>
    string(7) "example"
  }
  ["raw"]=>
  array(8) {
    ["scheme"]=>
    string(5) "https"
    ["username"]=>
    string(9) "%65xample"
    ["password"]=>
    string(9) "%65xample"
    ["host"]=>
    string(13) "%65xample.com"
    ["port"]=>
    int(123)
    ["path"]=>
    string(15) "/%65xample.html"
    ["query"]=>
    string(19) "%65xample=%65xample"
    ["fragment"]=>
    string(9) "%65xample"
  }
  ["struct"]=>
  array(8) {
    ["scheme"]=>
    string(5) "https"
    ["username"]=>
    string(9) "%65xample"
    ["password"]=>
    string(9) "%65xample"
    ["host"]=>
    string(13) "%65xample.com"
    ["port"]=>
    int(123)
    ["path"]=>
    string(15) "/%65xample.html"
    ["query"]=>
    string(19) "%65xample=%65xample"
    ["fragment"]=>
    string(9) "%65xample"
  }
}
